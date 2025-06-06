#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Robot.generated.h"

UCLASS()
class SNIPERSIMULATOR_API ARobot : public AActor
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    bool bIsWalking = false;

public:
    ARobot();

    FORCEINLINE void SetIsWalking(bool bInIsWalking) { bIsWalking = bInIsWalking; }
    FORCEINLINE bool IsWalking() { return bIsWalking; }

    FVector Direction = FVector::ZeroVector;

    UPROPERTY(BlueprintReadWrite)
    bool bIsStopped = false;

    UFUNCTION(BlueprintImplementableEvent)
    void BP_SetCapsuleForwardDistance(float Distance);
};
