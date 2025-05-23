#include "SettingsManager.h"
#include "Engine/PostProcessVolume.h"

ASettingsManager::ASettingsManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ASettingsManager::SetHourOfDay(EHourOfDay InHourOfDay)
{
    switch (InHourOfDay)
    {
    case EHourOfDay::DAY:
        DayLight->SetActorHiddenInGame(false);
        DaySky->SetActorHiddenInGame(false);
        NightSky->SetActorHiddenInGame(true);
        break;
    case EHourOfDay::NIGHT:
        DayLight->SetActorHiddenInGame(true);
        DaySky->SetActorHiddenInGame(true);
        NightSky->SetActorHiddenInGame(false);
        break;
    default:
        break;
    }
}

void ASettingsManager::SetVisionMode(EVisionMode InVisionMode)
{
    switch (InVisionMode)
    {
    case EVisionMode::DEFAULT:
        NightVisionPostProcess->bEnabled = false;
        TacticalVisionPostProcess->bEnabled = false;
        break;
    case EVisionMode::NIGHT:
        NightVisionPostProcess->bEnabled = true;
        TacticalVisionPostProcess->bEnabled = false;
        break;
    case EVisionMode::TACTICAL:
        NightVisionPostProcess->bEnabled = false;
        TacticalVisionPostProcess->bEnabled = true;
        break;
    default:
        break;
    }
}
