#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RobotsManager.generated.h"

class ARobot;
class USniperPlayerAnimInstance;

USTRUCT()
struct FRobotData
{
    GENERATED_BODY()

    TObjectPtr<ARobot> SpawnedActor = nullptr;
    TArray<FVector> Path;
    int32 CurrentPathNextPointIndex = 0;
    float IdleTimer = 0;
};

UCLASS()
class SNIPERSIMULATOR_API ARobotsManager : public AActor
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AActor> RobotActorClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    int32 MinRobots;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    int32 MaxRobots;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    FVector MinRobotPosition;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    FVector MaxRobotPosition;

    TArray<FRobotData> Robots;

public:
    ARobotsManager();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
};
