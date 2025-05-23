#include "SettingsManager.h"

ASettingsManager::ASettingsManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASettingsManager::BeginPlay()
{
    Super::BeginPlay();
}

void ASettingsManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}