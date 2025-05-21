#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LineDrawerWidget.generated.h"

class ASniperPlayer;
class ASniperSimulatorGameState;

UCLASS()
class SNIPERSIMULATOR_API ULineDrawerWidget : public UUserWidget
{
	GENERATED_BODY()
	
	ASniperPlayer* SniperPlayer = nullptr;
	ASniperSimulatorGameState* GameState = nullptr;
	APlayerController* PlayerController = nullptr;

public:
	void SetData(ASniperPlayer* InSniperPlayer, ASniperSimulatorGameState* InGameState, APlayerController* InPlayerController);

	virtual void NativeDestruct() override;

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
};
