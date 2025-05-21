#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SniperAimingWidget.generated.h"

class UTextBlock;
class UImage;
class ULineDrawerWidget;
class ASniperPlayer;
class ASniperSimulatorGameState;

UCLASS()
class SNIPERSIMULATOR_API USniperAimingWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget), DisplayName = "Target Distance")
	TObjectPtr<UTextBlock> TargetDistance = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget), DisplayName = "Wind Speed")
	TObjectPtr<UTextBlock> WindSpeed = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget), DisplayName = "Zoom Level")
	TObjectPtr<UTextBlock> ZoomLevel = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget), DisplayName = "Elevation")
	TObjectPtr<UTextBlock> Elevation = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget), DisplayName = "Wind Correction")
	TObjectPtr<UTextBlock> WindCorrection = nullptr;

	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget), DisplayName = "Wind Correction")
	TObjectPtr<UImage> ImpactPointImage = nullptr;

	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget), DisplayName = "Line Drawer Widget")
	TObjectPtr<ULineDrawerWidget> LineDrawerWidget = nullptr;

	ASniperPlayer* SniperPlayer = nullptr;
	ASniperSimulatorGameState* GameState = nullptr;
	APlayerController* PlayerController = nullptr;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void SetBindings();
	void ResetBindings();

private:
	UFUNCTION()
	void UpdateTargetDistance(float NewTargetDistance);
	UFUNCTION()
	void UpdateCurrentWind(float NewWindSpeed, float NewWindAngle);
	UFUNCTION()
	void UpdateZoomLevel(int32 NewZoomLevel);
	UFUNCTION()
	void UpdateElevation(int32 NewElevation);
	UFUNCTION()
	void UpdateWindCorrection(int32 NewWindCorrection);
};
