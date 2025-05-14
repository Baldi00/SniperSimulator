#include "SniperSimulatorGameState.h"
#include "Kismet/GameplayStatics.h"

ASniperSimulatorGameState::ASniperSimulatorGameState()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASniperSimulatorGameState::BeginPlay()
{
    Super::BeginPlay();
}

void ASniperSimulatorGameState::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    WindSpeedNoise = 0.25f * FMath::PerlinNoise1D(UGameplayStatics::GetRealTimeSeconds(this));
    WindAngleNoise = 10 * FMath::PerlinNoise1D(UGameplayStatics::GetRealTimeSeconds(this) + 5);
    OnWindUpdated.Broadcast(WindSpeed + WindSpeedNoise, GetWindAngleRelativeToPlayer());
}

float ASniperSimulatorGameState::GetWindAngleRelativeToPlayer()
{
    float PlayerAngle = UGameplayStatics::GetPlayerController(this, 0)->GetControlRotation().Yaw;
    if (PlayerAngle > 180)
        PlayerAngle -= 360;

    return WindAngle + WindAngleNoise - PlayerAngle;
}
