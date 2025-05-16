#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class SNIPERSIMULATOR_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	ABullet();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_HideMeshAndStopCamera();
};
