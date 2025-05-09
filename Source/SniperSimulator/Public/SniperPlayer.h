#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "EPlayerPoseState.h"
#include "SniperPlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class USniperPlayerAnimInstance;
class USniperAimingWidget;

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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    float StandingWalkSpeed = 180.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    float CrouchedWalkSpeed = 120.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    float ProneWalkSpeed = 50.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HUD, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<USniperAimingWidget> AimingWidgetClass;

    UPROPERTY()
    TObjectPtr<USniperPlayerAnimInstance> Animator = nullptr;
    UPROPERTY()
    TObjectPtr<USniperAimingWidget> AimingWidget = nullptr;
    
    bool bIsAiming = false;
    EPlayerPoseState PlayerState = EPlayerPoseState::STANDING;
    FTimerHandle DefaultToAimingTimerHandler;

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

    void SetPlayerPoseState(EPlayerPoseState NewPlayerPoseState);
    void SwitchToDefaultView();
    void SwitchToAimingView();
};
