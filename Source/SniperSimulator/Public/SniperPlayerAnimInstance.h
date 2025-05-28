#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EPlayerPoseState.h"
#include "SniperPlayerAnimInstance.generated.h"

UCLASS()
class SNIPERSIMULATOR_API USniperPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsAiming;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsWalking;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsRunning;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPlayerPoseState PlayerPoseState;
};
