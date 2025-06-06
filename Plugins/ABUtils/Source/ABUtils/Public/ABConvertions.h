#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ABConvertions.generated.h"

class USceneCaptureComponent2D;

UCLASS()
class ABUTILS_API UABConvertions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "AB Convertions Utils")
	static FVector2D WorldToScreenPoint(const FVector& WorldLocation, const USceneCaptureComponent2D* SceneCapture);
};
