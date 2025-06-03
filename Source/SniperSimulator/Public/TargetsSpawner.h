#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetsSpawner.generated.h"

UCLASS()
class SNIPERSIMULATOR_API ATargetsSpawner : public AActor
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AActor> PumpkinActorClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    int32 MinPumpkins;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    int32 MaxPumpkins;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    FVector MinPumpkinPosition;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    FVector MaxPumpkinPosition;

public:
    ATargetsSpawner();

protected:
    virtual void BeginPlay() override;
};