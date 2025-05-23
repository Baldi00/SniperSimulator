#include "SniperPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SniperPlayerAnimInstance.h"
#include "Blueprint/UserWidget.h"
#include "SniperAimingWidget.h"
#include "SniperSimulatorGameState.h"
#include "Kismet/GameplayStatics.h"
#include <ABDebug.h>
#include "BulletHittableItem.h"

ASniperPlayer::ASniperPlayer()
{
    PrimaryActorTick.bCanEverTick = true;

    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Create a camera boom
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 440.0f;
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bDoCollisionTest = true;

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // Create a camera boom
    CameraBoomAiming = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoomAiming"));
    CameraBoomAiming->SetupAttachment(RootComponent);
    CameraBoomAiming->TargetArmLength = 0;
    CameraBoomAiming->SocketOffset = FVector(0, 10, 60);
    CameraBoomAiming->bUsePawnControlRotation = true;
    CameraBoomAiming->bDoCollisionTest = false;

    // Create a follow camera
    AimingCameraActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("AimingCameraActor"));
    AimingCameraActor->SetupAttachment(CameraBoomAiming, USpringArmComponent::SocketName);
}

void ASniperPlayer::BeginPlay()
{
    Super::BeginPlay();

    if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
            Subsystem->AddMappingContext(DefaultMappingContext, 0);

    Animator = Cast<USniperPlayerAnimInstance>(GetMesh()->GetAnimInstance());
    AimingCameraComponent = AimingCameraActor->GetChildActor()->FindComponentByClass<UCameraComponent>();
    AimingCameraComponent->bConstrainAspectRatio = false;

    GameState = Cast<ASniperSimulatorGameState>(UGameplayStatics::GetGameState(this));
}

void ASniperPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    switch (PlayerState)
    {
    case EPlayerPoseState::STANDING:
        CameraBoom->TargetOffset = FMath::VInterpTo(CameraBoom->TargetOffset, FVector::ZeroVector, DeltaTime, 10);
        CameraBoomAiming->TargetOffset = FMath::VInterpTo(CameraBoomAiming->TargetOffset, FVector::ZeroVector, DeltaTime, 10);
        break;
    case EPlayerPoseState::CROUCHED:
        CameraBoom->TargetOffset = FMath::VInterpTo(CameraBoom->TargetOffset, FVector(0, 0, -30), DeltaTime, 10);
        CameraBoomAiming->TargetOffset = FMath::VInterpTo(CameraBoomAiming->TargetOffset, FVector(0, 0, -50), DeltaTime, 10);
        break;
    case EPlayerPoseState::PRONE:
        CameraBoom->TargetOffset = FMath::VInterpTo(CameraBoom->TargetOffset, FVector(0, 0, -60), DeltaTime, 10);
        CameraBoomAiming->TargetOffset = FMath::VInterpTo(CameraBoomAiming->TargetOffset, FVector(0, 0, -100), DeltaTime, 10);
        break;
    default:
        break;
    }

    AimingCameraComponent->FieldOfView = FMath::FInterpTo(AimingCameraComponent->FieldOfView, GetCurrentZoomFieldOfView(), DeltaTime, 10);

    if (bIsAiming)
    {
        FHitResult HitResult;
        UKismetSystemLibrary::LineTraceSingle(this, AimingCameraComponent->GetComponentLocation(), AimingCameraComponent->GetComponentLocation() + AimingCameraComponent->GetForwardVector() * 300000, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true);
        CurrentTargetDistance = HitResult.Distance / 100;
        OnTargetDistandceUpdated.Broadcast(CurrentTargetDistance);

        if (GameState)
        {
            GameState->ComputeTrajectory(AimingCameraComponent->GetComponentLocation(), GetControlRotation());
            GameState->ComputeImpactPoint();
        }

        if (!bIsStabilizedAiming)
        {
            float OscillationStrength = 0;
            switch (PlayerState)
            {
            case EPlayerPoseState::STANDING:
                OscillationStrength = 10;
                break;
            case EPlayerPoseState::CROUCHED:
                OscillationStrength = 5;
                break;
            case EPlayerPoseState::PRONE:
                OscillationStrength = 2;
                break;
            default:
                break;
            }

            OscillationStrength *= FMath::Lerp(1.f, 10.f, (float)CurrentZoomIndex / ZoomLevels.Num());
            AimingCameraActor->SetRelativeLocation(FVector(0, FMath::PerlinNoise1D(UGameplayStatics::GetRealTimeSeconds(this) + 17) * OscillationStrength, FMath::PerlinNoise1D(UGameplayStatics::GetRealTimeSeconds(this) + 24) * OscillationStrength));
        }
    }

    if (bIsShooting)
    {
        if (BulletShootingTimer >= BulletShootingDuration)
            ShootingEnded();
        else
        {
            UpdateSpawnedBulletTransform(BulletShootingTimer);
            BulletShootingTimer += DeltaTime;
        }
    }
}

void ASniperPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASniperPlayer::Move);
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ASniperPlayer::StopMoving);
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASniperPlayer::Look);
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ASniperPlayer::ToggleCrouching);
        EnhancedInputComponent->BindAction(ProneAction, ETriggerEvent::Completed, this, &ASniperPlayer::ToggleProne);
        EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ASniperPlayer::StartAiming);
        EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ASniperPlayer::StopAiming);
        EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ASniperPlayer::Zoom);
        EnhancedInputComponent->BindAction(ElevationRegulationAction, ETriggerEvent::Triggered, this, &ASniperPlayer::RegolateElevation);
        EnhancedInputComponent->BindAction(WindageRegulationAction, ETriggerEvent::Triggered, this, &ASniperPlayer::RegolateWindage);
        EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ASniperPlayer::Shoot);
        EnhancedInputComponent->BindAction(ShowShootingTableAction, ETriggerEvent::Started, this, &ASniperPlayer::ShowShootingTable);
        EnhancedInputComponent->BindAction(ShowShootingTableAction, ETriggerEvent::Completed, this, &ASniperPlayer::HideShootingTable);
        EnhancedInputComponent->BindAction(StabilizeAimingAction, ETriggerEvent::Started, this, &ASniperPlayer::StabilizeAiming);
        EnhancedInputComponent->BindAction(StabilizeAimingAction, ETriggerEvent::Completed, this, &ASniperPlayer::StabilizeAiming);
        EnhancedInputComponent->BindAction(TeleportAction, ETriggerEvent::Triggered, this, &ASniperPlayer::TeleportLogic);
        EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Triggered, this, &ASniperPlayer::PauseGame);
    }
}

void ASniperPlayer::Move(const FInputActionValue& Value)
{
    if (bIsInKillcam)
        return;

    const FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        AddMovementInput(ForwardDirection, MovementVector.Y);
        AddMovementInput(RightDirection, MovementVector.X);

        Animator->bIsWalking = true;
    }
}

void ASniperPlayer::StopMoving(const FInputActionValue& Value)
{
    Animator->bIsWalking = false;
}

void ASniperPlayer::Look(const FInputActionValue& Value)
{
    if (bIsInKillcam)
        return;

    const FVector2D LookAxisVector = Value.Get<FVector2D>();

    float Sensitivity = 1;
    if (bIsAiming)
        Sensitivity = 1.f / ZoomLevels[CurrentZoomIndex];

    if (Controller != nullptr)
    {
        AddControllerYawInput(LookAxisVector.X * Sensitivity);
        AddControllerPitchInput(LookAxisVector.Y * Sensitivity);
    }
}

void ASniperPlayer::ToggleCrouching(const FInputActionValue& Value)
{
    if (bIsInKillcam)
        return;

    switch (PlayerState)
    {
    case EPlayerPoseState::STANDING:
        SetPlayerPoseState(EPlayerPoseState::CROUCHED);
        break;
    case EPlayerPoseState::CROUCHED:
        SetPlayerPoseState(EPlayerPoseState::STANDING);
        break;
    case EPlayerPoseState::PRONE:
        SetPlayerPoseState(EPlayerPoseState::CROUCHED);
        break;
    default:
        break;
    }
}

void ASniperPlayer::ToggleProne(const FInputActionValue& Value)
{
    if (bIsInKillcam)
        return;

    switch (PlayerState)
    {
    case EPlayerPoseState::STANDING:
    case EPlayerPoseState::CROUCHED:
        SetPlayerPoseState(EPlayerPoseState::PRONE);
        break;
    case EPlayerPoseState::PRONE:
        SetPlayerPoseState(EPlayerPoseState::STANDING);
        break;
    default:
        break;
    }
}

void ASniperPlayer::StartAiming(const FInputActionValue& Value)
{
    if (bIsAiming)
        return;

    bIsAiming = true;
    Animator->bIsAiming = bIsAiming;

    if (bIsInKillcam)
        return;

    FHitResult UnusedHit;
    GetCharacterMovement()->SafeMoveUpdatedComponent(FVector::ZeroVector, FRotator(0, GetControlRotation().Yaw, 0), false, UnusedHit);
    UGameplayStatics::GetPlayerController(this, 0)->SetViewTargetWithBlend(AimingCameraActor->GetChildActor(), 0.2f);
    GetWorldTimerManager().SetTimer(DefaultToAimingTimerHandler, this, &ASniperPlayer::SwitchToAimingView, 0.2f);
}

void ASniperPlayer::StopAiming(const FInputActionValue& Value)
{
    if (!bIsAiming)
        return;

    GetWorldTimerManager().ClearTimer(DefaultToAimingTimerHandler);
    bIsAiming = false;
    Animator->bIsAiming = bIsAiming;

    if (bIsInKillcam)
        return;

    FHitResult UnusedHit;
    GetCharacterMovement()->SafeMoveUpdatedComponent(FVector::ZeroVector, FRotator(0, GetControlRotation().Yaw, 0), false, UnusedHit);
    SwitchToDefaultView();
    UGameplayStatics::GetPlayerController(this, 0)->SetViewTargetWithBlend(this, 0.2f);
}

void ASniperPlayer::Zoom(const FInputActionValue& Value)
{
    if (!bIsAiming || bIsInKillcam)
        return;

    const float Input = Value.Get<float>();

    if (Input > 0)
        CurrentZoomIndex++;
    else if (Input < 0)
        CurrentZoomIndex--;

    int32 UnclampedZoomIndex = CurrentZoomIndex;
    CurrentZoomIndex = FMath::Clamp(CurrentZoomIndex, 0, ZoomLevels.Num() - 1);

    if (UnclampedZoomIndex == CurrentZoomIndex)
    {
        OnZoomLevelUpdated.Broadcast(ZoomLevels[CurrentZoomIndex]);
        BP_PlayClickSound();
    }
}

void ASniperPlayer::RegolateElevation(const FInputActionValue& Value)
{
    if (!bIsAiming || bIsInKillcam)
        return;

    const float Input = Value.Get<float>();

    if (Input > 0)
        CurrentElevationLevel--;
    else if (Input < 0)
        CurrentElevationLevel++;

    AimingCameraComponent->SetRelativeRotation(FRotator(CurrentElevationLevel * ClickAngle, CurrentWindageLevel * ClickAngle, 0));
    BP_PlayClickSound();
    OnElevationRegulationUpdated.Broadcast(CurrentElevationLevel);
}

void ASniperPlayer::RegolateWindage(const FInputActionValue& Value)
{
    if (!bIsAiming || bIsInKillcam)
        return;

    const float Input = Value.Get<float>();

    if (Input > 0)
        CurrentWindageLevel--;
    else if (Input < 0)
        CurrentWindageLevel++;

    AimingCameraComponent->SetRelativeRotation(FRotator(CurrentElevationLevel * ClickAngle, CurrentWindageLevel * ClickAngle, 0));
    BP_PlayClickSound();
    OnWindageRegulationUpdated.Broadcast(CurrentWindageLevel);
}

void ASniperPlayer::Shoot(const FInputActionValue& Value)
{
    if (!bIsAiming || bIsShooting || bIsInKillcam)
        return;

    if (SpawnedBulletActor != nullptr)
        SpawnedBulletActor->Destroy();

    GameState->SaveShootData();

    BP_PlayShootSound();
    SpawnAndStartMovingBullet();
}

void ASniperPlayer::ShowShootingTable(const FInputActionValue& Value)
{
    if (bIsShooting || bIsInKillcam)
        return;
    ShootingTableWidget = CreateWidget<UUserWidget>(GetWorld(), ShootingTableWidgetClass);
    ShootingTableWidget->AddToViewport(100);
}

void ASniperPlayer::HideShootingTable(const FInputActionValue& Value)
{
    if (ShootingTableWidget != nullptr)
        ShootingTableWidget->RemoveFromParent();
}

void ASniperPlayer::StabilizeAiming(const FInputActionValue& Value)
{
    if (!bIsAiming)
        return;

    bIsStabilizedAiming = Value.Get<bool>();
    if (bIsStabilizedAiming)
        BP_PlayBreathSound();
}

void ASniperPlayer::TeleportLogic(const FInputActionValue& Value)
{
    if (!bIsAiming || bIsShooting || bIsInKillcam)
        return;

    // Teleport
    FHitResult HitResult;
    UKismetSystemLibrary::LineTraceSingle(this, AimingCameraComponent->GetComponentLocation(), AimingCameraComponent->GetComponentLocation() + AimingCameraComponent->GetForwardVector() * 300000, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true);
    if (HitResult.bBlockingHit)
    {
        FVector DirectionXY = AimingCameraComponent->GetForwardVector();
        DirectionXY.Z = 0;
        DirectionXY.Normalize();
        UKismetSystemLibrary::LineTraceSingle(this, HitResult.ImpactPoint + FVector::UpVector * 10000 + DirectionXY * 10, HitResult.ImpactPoint + DirectionXY * 10 - FVector::UpVector * 100, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true);
        if (HitResult.bBlockingHit)
            SetActorLocation(HitResult.ImpactPoint + FVector::UpVector * 100);
    }
}

void ASniperPlayer::PauseGame(const FInputActionValue& Value)
{
    UUserWidget* SettingsPauseWidget = CreateWidget<UUserWidget>(GetWorld(), SettingsPauseWidgetClass);
    SettingsPauseWidget->AddToViewport(500);
}

void ASniperPlayer::SetPlayerPoseState(EPlayerPoseState NewPlayerPoseState)
{
    PlayerState = NewPlayerPoseState;

    switch (PlayerState)
    {
    case EPlayerPoseState::STANDING:
        GetCharacterMovement()->MaxWalkSpeed = StandingWalkSpeed;
        break;
    case EPlayerPoseState::CROUCHED:
        GetCharacterMovement()->MaxWalkSpeed = CrouchedWalkSpeed;
        break;
    case EPlayerPoseState::PRONE:
        GetCharacterMovement()->MaxWalkSpeed = ProneWalkSpeed;
        break;
    default:
        break;
    }

    Animator->PlayerPoseState = PlayerState;
}

void ASniperPlayer::SwitchToDefaultView()
{
    GetMesh()->SetVisibility(true, true);
    if (AimingWidget != nullptr)
    {
        AimingWidget->RemoveFromParent();
        AimingWidget = nullptr;
    }
}

void ASniperPlayer::SwitchToAimingView()
{
    GetWorldTimerManager().ClearTimer(DefaultToAimingTimerHandler);
    if (AimingWidgetClass && AimingWidget == nullptr)
    {
        AimingWidget = CreateWidget<USniperAimingWidget>(GetWorld(), AimingWidgetClass);
        AimingWidget->AddToViewport();
    }
    GetMesh()->SetVisibility(false, true);
}

void ASniperPlayer::UpdateSpawnedBulletTransform(float Time)
{
    if (SpawnedBulletActor != nullptr)
    {
        TArray<AActor*> Ignored;
        FVector NextPosition = UBulletTrajectoryCalculator::GetPositionAtTime(GameState->GetShootedTrajectory(), GameState->GetShotSimulationTimeIntervalSeconds(), Time);
        FHitResult HitResult;
        UKismetSystemLibrary::LineTraceSingle(this, SpawnedBulletActor->GetActorLocation(), NextPosition, ETraceTypeQuery::TraceTypeQuery1, false, Ignored, EDrawDebugTrace::None, HitResult, true);
        if (HitResult.bBlockingHit)
        {
            if (HitResult.GetActor()->Implements<UBulletHittableItem>())
            {
                IBulletHittableItem::Execute_BulletHit(HitResult.GetActor(), (NextPosition - SpawnedBulletActor->GetActorLocation()).GetSafeNormal(), bIsInKillcam);
                if (!bIsInKillcam)
                    BP_PlayFarHitSound();
            }
            ShootingEnded();
        }
        else
            SpawnedBulletActor->SetActorLocation(NextPosition);
    }
}

void ASniperPlayer::ShootingEnded()
{
    if (bIsInKillcam)
    {
        if (bIsStabilizedAiming)
            UGameplayStatics::SetGlobalTimeDilation(this, 0.8f);
        else
            UGameplayStatics::SetGlobalTimeDilation(this, 1);
        SpawnedBulletActor->BP_HideMeshAndStopCamera();
        FTimerHandle Unused;
        GetWorldTimerManager().SetTimer(Unused, this, &ASniperPlayer::ShootingEndedKillcam, 2);
    }
    else
    {
        SetIsShooting(false);
        if (SpawnedBulletActor != nullptr)
        {
            SpawnedBulletActor->Destroy();
            SpawnedBulletActor = nullptr;
        }
    }
}

void ASniperPlayer::ShootingEndedKillcam()
{
    bIsInKillcam = false;
    if (bIsAiming)
    {
        UGameplayStatics::GetPlayerController(this, 0)->SetViewTarget(AimingCameraActor->GetChildActor());
        SwitchToAimingView();
    }
    else
    {
        UGameplayStatics::GetPlayerController(this, 0)->SetViewTarget(this);
    }
    ShootingEnded();
}

void ASniperPlayer::StartKillcam()
{
    bIsInKillcam = true;
    UGameplayStatics::SetGlobalTimeDilation(this, 0.25f);
    HideShootingTable(FInputActionValue());
    SwitchToDefaultView();
    UGameplayStatics::GetPlayerController(this, 0)->SetViewTarget(SpawnedBulletActor);
}

void ASniperPlayer::SpawnAndStartMovingBullet()
{
    SpawnedBulletActor = GetWorld()->SpawnActor<ABullet>(BulletActorClass, AimingCameraComponent->GetComponentLocation(), GetControlRotation());

    BulletShootingTimer = 0;
    BulletShootingDuration = GameState->GetShotSimulationTimeSeconds();

    SetIsShooting(true);
    if (GameState->GetUseKillcam())
        StartKillcam();
}