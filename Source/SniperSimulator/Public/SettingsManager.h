#pragma once

#include "CoreMinimal.h"
#include "SettingsManager.generated.h"

UCLASS()
class SNIPERSIMULATOR_API ASettingsManager : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Bullet, meta = (AllowPrivateAccess = "true"))
	float Test = 0;

public:
	ASettingsManager();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
