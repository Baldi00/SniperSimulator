#pragma once

#include "CoreMinimal.h"
#include "SniperSimulatorGameState.h"
#include "SettingsManager.generated.h"

UCLASS()
class SNIPERSIMULATOR_API ASettingsManager : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> DayLight;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> DaySky;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> NightSky;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<APostProcessVolume> NightVisionPostProcess;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<APostProcessVolume> TacticalVisionPostProcess;

public:
	ASettingsManager();

	void SetHourOfDay(EHourOfDay InHourOfDay);
	void SetVisionMode(EVisionMode InVisionMode);
};
