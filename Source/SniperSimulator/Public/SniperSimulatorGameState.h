#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SniperSimulatorGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWindUpdated, float, InCurrentWindSpeed, float, InCurrentWindAngle);

UCLASS()
class SNIPERSIMULATOR_API ASniperSimulatorGameState : public AGameStateBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float WindSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float WindAngle;

	float WindSpeedNoise = 0;
	float WindAngleNoise = 0;

public:
	ASniperSimulatorGameState();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE float GetWindSpeed() const { return WindSpeed + WindSpeedNoise; }
	FORCEINLINE float GetWindAngle() const { return WindAngle + WindAngleNoise; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetWindAngleRelativeToPlayer();

	FOnWindUpdated OnWindUpdated;
};
