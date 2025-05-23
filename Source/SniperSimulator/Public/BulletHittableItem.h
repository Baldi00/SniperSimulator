#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BulletHittableItem.generated.h"

UINTERFACE(MinimalAPI)
class UBulletHittableItem : public UInterface
{
    GENERATED_BODY()
};

class SNIPERSIMULATOR_API IBulletHittableItem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent)
    void BulletHit(FVector InDirection, bool bInIsKillcam);

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    bool IsAlreadyHit();
};