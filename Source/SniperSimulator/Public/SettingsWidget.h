#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsWidget.generated.h"

class UCheckBox;
class UEditableTextBox;
class UButton;

class ASniperPlayer;
class ASniperSimulatorGameState;

UCLASS()
class SNIPERSIMULATOR_API USettingsWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCheckBox> DayCheckbox = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCheckBox> NightCheckbox = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCheckBox> DefaultVisionCheckbox = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCheckBox> NightVisionCheckbox = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCheckBox> TacticalVisionCheckbox = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCheckBox> ShowImpactPointCheckbox = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCheckBox> ShowTrajectoryCheckbox = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCheckBox> UseSilencerCheckbox = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCheckBox> KillcamCheckbox = nullptr;

	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEditableTextBox> MassTextBox = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEditableTextBox> CaliberTextBox = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEditableTextBox> DragCoefficientTextBox = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEditableTextBox> ShotSpeedTextBox = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEditableTextBox> HeightAboveSeaLevelTextBox = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEditableTextBox> TemperatureTextBox = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEditableTextBox> WindSpeedTextBox = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEditableTextBox> WindAngleTextBox = nullptr;

	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> RandomizeWindButton = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> SaveButton = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> ResetButton = nullptr;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> QuitGameButton = nullptr;

	ASniperPlayer* SniperPlayer = nullptr;
	ASniperSimulatorGameState* GameState = nullptr;
	APlayerController* PlayerController = nullptr;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetBindings();
	void ResetBindings();

	void PopulateUI();

	UFUNCTION()
	void RandomizeWind();
	UFUNCTION()
	void SaveAndClose();
	UFUNCTION()
	void ResetSettings();
	UFUNCTION()
	void CloseSettings();
	UFUNCTION()
	void QuitGame();
};
