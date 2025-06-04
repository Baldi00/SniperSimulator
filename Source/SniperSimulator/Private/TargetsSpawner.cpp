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
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
            if (SpawnedPumpkin)
                SpawnedPumpkin->SetFolderPath("Pumpkins");
#endif
        }
    }
}