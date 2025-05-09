#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ABMath.generated.h"

UCLASS()
class ABUTILS_API UABMath : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, BlueprintCallable, Category="AB Math Utils")
	static float AngleBetweenVectors(const FVector& InVector1, const FVector& InVector2);
	
	UFUNCTION(BlueprintPure, BlueprintCallable, Category="AB Math Utils")
	static float InverseLerp(float min, float max, float val);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "AB Math Utils")
	static float Remap(float iMin, float iMax, float oMin, float oMax, float val);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "AB Math Utils")
	static FVector ComputeBulletInitialVelocity(const FVector& SpawnLocation, const FVector& TargetLocation, float TimeToReach);
};
