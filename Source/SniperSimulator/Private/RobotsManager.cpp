#include "RobotsManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "RobotAnimInstance.h"
#include "Robot.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SniperSimulatorGameState.h"
#include "SniperPlayer.h"
#include "Camera/CameraComponent.h"

ARobotsManager::ARobotsManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ARobotsManager::BeginPlay()
{
    Super::BeginPlay();

    GameState = Cast<ASniperSimulatorGameState>(UGameplayStatics::GetGameState(this));
    SniperPlayer = Cast<ASniperPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));;

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    // Robots
    int32 RobotsCount = FMath::RandRange(MinRobots, MaxRobots);
    for (int i = 0; i < RobotsCount; i++)
    {
        FVector Position = FVector(FMath::RandRange(MinRobotPosition.X, MaxRobotPosition.X), FMath::RandRange(MinRobotPosition.Y, MaxRobotPosition.Y), MaxRobotPosition.Z);
        FHitResult HitResult;
        UKismetSystemLibrary::LineTraceSingle(this, Position, Position - FVector::UpVector * (MaxRobotPosition.Z - MinRobotPosition.Z), ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true);
        if (HitResult.bBlockingHit)
        {
            ARobot* SpawnedRobot = GetWorld()->SpawnActor<ARobot>(RobotActorClass, HitResult.ImpactPoint, FRotator::ZeroRotator, SpawnParameters);
            if (SpawnedRobot)
            {
#if UE_EDITOR
                SpawnedRobot->SetFolderPath("Robots");
#endif
                Robots.Add({ SpawnedRobot, Cast<URobotAnimInstance>(SpawnedRobot->GetComponentByClass<USkeletalMeshComponent>()->GetAnimInstance()) });
            }
        }
    }
}

void ARobotsManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    for (FRobotData& Robot : Robots)
    {
        if (Robot.SpawnedActor->bIsStopped)
            continue;

        if (Robot.IdleTimer <= 0)
        {
            if (Robot.Path.Num() == 0)
            {
                // Select first path
                FVector Destination;
                UNavigationSystemV1::K2_GetRandomReachablePointInRadius(this, Robot.SpawnedActor->GetActorLocation(), Destination, 3000);
                UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, Robot.SpawnedActor->GetActorLocation(), Destination);
                if (NavPath)
                {
                    Robot.Path = NavPath->PathPoints;
                    Robot.CurrentPathNextPointIndex = 0;
                }
            }
            else if (FVector::DistSquared(Robot.SpawnedActor->GetActorLocation(), Robot.Path[Robot.CurrentPathNextPointIndex]) <= 225)
            {
                if (Robot.CurrentPathNextPointIndex < Robot.Path.Num() - 1)
                {
                    // Update NextPoint
                    Robot.CurrentPathNextPointIndex++;
                }
                else
                {
                    // Set idle & select next path
                    Robot.IdleTimer = FMath::RandRange(3.f, 6.f);
                    FVector Destination;
                    UNavigationSystemV1::K2_GetRandomReachablePointInRadius(this, Robot.SpawnedActor->GetActorLocation(), Destination, 3000);
                    UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, Robot.SpawnedActor->GetActorLocation(), Destination);
                    if (NavPath)
                    {
                        Robot.Path = NavPath->PathPoints;
                        Robot.CurrentPathNextPointIndex = 0;
                    }
                    Robot.Animator->bIsWalking = false;
                    Robot.SpawnedActor->BP_SetCapsuleForwardDistance(0);
                }
            }
            else
            {
                // Go to next point
                FVector Direction = (Robot.Path[Robot.CurrentPathNextPointIndex] - Robot.SpawnedActor->GetActorLocation()).GetSafeNormal();
                FRotator ActorRotation = UKismetMathLibrary::FindLookAtRotation(Robot.SpawnedActor->GetActorLocation(), Robot.Path[Robot.CurrentPathNextPointIndex]);
                ActorRotation.Pitch = 0;
                ActorRotation.Roll = 0;
                Robot.SpawnedActor->SetActorRotation(ActorRotation);
                Robot.SpawnedActor->AddActorWorldOffset(180.f * DeltaTime * Direction);
                Robot.SpawnedActor->Direction = Direction;
                Robot.Animator->bIsWalking = true;

                // Time to reach collider
                float TimeOfFlight = UBulletTrajectoryCalculator::GetTrajectoryPointDataAtDistance(GameState->GetCurrentTrajectory(), SniperPlayer->GetAimingCameraComponent()->GetComponentLocation(), SniperPlayer->GetControlRotation(), GameState->GetShotSimulationTimeIntervalSeconds(), FVector::Dist(SniperPlayer->GetActorLocation(), Robot.SpawnedActor->GetActorLocation())).TimeOfFlight;
                Robot.SpawnedActor->BP_SetCapsuleForwardDistance(180.f * TimeOfFlight);
            }
        }
        else
        {
            // Decrease idle
            Robot.IdleTimer -= DeltaTime;
        }
    }
}

