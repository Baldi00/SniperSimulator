#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RobotsManager.generated.h"

class ARobot;
class USniperPlayerAnimInstance;
class URobotAnimInstance;
class ASniperSimulatorGameState;
class ASniperPlayer;

USTRUCT()
struct FRobotData
{
    GENERATED_BODY()

    TObjectPtr<ARobot> SpawnedActor = nullptr;
    TObjectPtr<URobotAnimInstance> Animator = nullptr;
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

    ASniperSimulatorGameState* GameState = nullptr;
    ASniperPlayer* SniperPlayer = nullptr;

public:
    ARobotsManager();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
};
