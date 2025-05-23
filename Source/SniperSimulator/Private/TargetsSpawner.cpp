#include "TargetsSpawner.h"
#include "Kismet/KismetSystemLibrary.h"

ATargetsSpawner::ATargetsSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ATargetsSpawner::BeginPlay()
{
    Super::BeginPlay();

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    // Pumpkins
    int32 PumpkinsCount = FMath::RandRange(MinPumpkins, MaxPumpkins);
    for (int i = 0; i < PumpkinsCount; i++)
    {
        FVector Position = FVector(FMath::RandRange(MinPumpkinPosition.X, MaxPumpkinPosition.X), FMath::RandRange(MinPumpkinPosition.Y, MaxPumpkinPosition.Y), MaxPumpkinPosition.Z);
        FHitResult HitResult;
        UKismetSystemLibrary::LineTraceSingle(this, Position, Position - FVector::UpVector * (MaxPumpkinPosition.Z - MinPumpkinPosition.Z), ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true);
        if (HitResult.bBlockingHit)
        {
            AActor* SpawnedPumpkin = GetWorld()->SpawnActor<AActor>(PumpkinActorClass, HitResult.ImpactPoint, FRotator::ZeroRotator, SpawnParameters);
            if (SpawnedPumpkin)
                SpawnedPumpkin->SetFolderPath("Pumpkins");
        }
    }

    // Robots
    int32 RobotsCount = FMath::RandRange(MinRobots, MaxRobots);
    for (int i = 0; i < RobotsCount; i++)
    {
        FVector Position = FVector(FMath::RandRange(MinRobotPosition.X, MaxRobotPosition.X), FMath::RandRange(MinRobotPosition.Y, MaxRobotPosition.Y), MaxRobotPosition.Z);
        FHitResult HitResult;
        UKismetSystemLibrary::LineTraceSingle(this, Position, Position - FVector::UpVector * (MaxRobotPosition.Z - MinRobotPosition.Z), ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true);
        if (HitResult.bBlockingHit)
        {
            AActor* SpawnedRobot = GetWorld()->SpawnActor<AActor>(RobotActorClass, HitResult.ImpactPoint, FRotator::ZeroRotator, SpawnParameters);
            if (SpawnedRobot)
                SpawnedRobot->SetFolderPath("Robots");
        }
    }
}