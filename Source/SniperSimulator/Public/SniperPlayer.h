#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "EPlayerPoseState.h"
#include "Bullet.h"
#include "SniperPlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class USniperPlayerAnimInstance;
class USniperAimingWidget;
class ASniperSimulatorGameState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZoomLevelUpdated, int32, InCurrentZoomLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetDistandceUpdated, float, InCurrentTargetDistance);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnElevationRegulationUpdated, int32, InCurrentElevationRegulation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWindageRegulationUpdated, int32, InCurrentWindageRegulation);

UCLASS()
class SNIPERSIMULATOR_API ASniperPlayer : public ACharacter
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> FollowCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USpringArmComponent> CameraBoomAiming;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UChildActorComponent> AimingCameraActor;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> CrouchAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> ProneAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> AimAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> ZoomAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> ElevationRegulationAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> WindageRegulationAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> ShootAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> ShowShootingTableAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    float StandingWalkSpeed = 180.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    float CrouchedWalkSpeed = 120.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    float ProneWalkSpeed = 50.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HUD, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<USniperAimingWidget> AimingWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HUD, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UUserWidget> ShootingTableWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SniperRifle, meta = (AllowPrivateAccess = "true"))
    TArray<int32> ZoomLevels;

    UPROPERTY()
    TObjectPtr<USniperPlayerAnimInstance> Animator = nullptr;
    UPROPERTY()
    TObjectPtr<USniperAimingWidget> AimingWidget = nullptr;
    UPROPERTY()
    TObjectPtr<UUserWidget> ShootingTableWidget = nullptr;

    bool bIsAiming = false;
    bool bIsShooting = false;
    bool bIsInKillcam = false;
    EPlayerPoseState PlayerState = EPlayerPoseState::STANDING;
    FTimerHandle DefaultToAimingTimerHandler;
    TObjectPtr<ASniperSimulatorGameState> GameState;

    TObjectPtr<UCameraComponent> AimingCameraComponent;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    int32 CurrentZoomIndex = 0;
    float OriginalFov = 90;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    float CurrentTargetDistance = 0;

    int32 CurrentElevationLevel = 0;
    int32 CurrentWindageLevel = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SniperRifle, meta = (AllowPrivateAccess = "true"))
    float ClickAngle;

    UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<ABullet> BulletActorClass = nullptr;
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ABullet> SpawnedBulletActor = nullptr;

    float BulletShootingTimer = 0;
    float BulletShootingDuration = 0;

public:
    ASniperPlayer();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    void Move(const FInputActionValue& Value);
    void StopMoving(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void ToggleCrouching(const FInputActionValue& Value);
    void ToggleProne(const FInputActionValue& Value);
    void StartAiming(const FInputActionValue& Value);
    void StopAiming(const FInputActionValue& Value);
    void Zoom(const FInputActionValue& Value);
    void RegolateElevation(const FInputActionValue& Value);
    void RegolateWindage(const FInputActionValue& Value);
    void Shoot(const FInputActionValue& Value);
    void ShowShootingTable(const FInputActionValue& Value);
    void HideShootingTable(const FInputActionValue& Value);

    void SetPlayerPoseState(EPlayerPoseState NewPlayerPoseState);
    void SwitchToDefaultView();
    void SwitchToAimingView();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    FORCEINLINE float GetCurrentZoomFieldOfView() { return FMath::RadiansToDegrees(2 * FMath::Atan(FMath::Tan(FMath::DegreesToRadians(OriginalFov / 2)) / ZoomLevels[CurrentZoomIndex])); }

    void UpdateSpawnedBulletTransform(float Time);
    void ShootingEnded();
    void ShootingEndedKillcam();

    void StartKillcam(float Duration);

    UFUNCTION(BlueprintImplementableEvent)
    void BP_PlayShootSound();

public:
    UPROPERTY(BlueprintAssignable)
    FOnZoomLevelUpdated OnZoomLevelUpdated;
    UPROPERTY(BlueprintAssignable)
    FOnTargetDistandceUpdated OnTargetDistandceUpdated;
    UPROPERTY(BlueprintAssignable)
    FOnElevationRegulationUpdated OnElevationRegulationUpdated;
    UPROPERTY(BlueprintAssignable)
    FOnWindageRegulationUpdated OnWindageRegulationUpdated;

    FORCEINLINE int32 GetCurrentZoomLevel() { return ZoomLevels[CurrentZoomIndex]; }
    FORCEINLINE float GetCurrentTargetDistance() { return CurrentTargetDistance; }
    FORCEINLINE int32 GetCurrentElevationLevel() { return CurrentElevationLevel; }
    FORCEINLINE int32 GetCurrentWindageLevel() { return CurrentWindageLevel; }

    FORCEINLINE void SetIsShooting(bool bInIsShooting) { bIsShooting = bInIsShooting; }
    FORCEINLINE AActor* GetSpawnedBulletActor() { return SpawnedBulletActor; }
};
