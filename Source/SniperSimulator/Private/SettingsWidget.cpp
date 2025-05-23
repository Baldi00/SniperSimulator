#include "SettingsWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Kismet/GameplayStatics.h"
#include "SniperSimulatorGameState.h"
#include "SniperPlayer.h"

void USettingsWidget::NativeConstruct()
{
    Super::NativeConstruct();
    SetBindings();
    ShowImpactPointCheckbox->SetCheckedState(GameState->bDrawHitPointEnabled ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
    ShowTrajectoryCheckbox->SetCheckedState(GameState->bDrawTrajectoryEnabled ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
    UseSilencerCheckbox->SetCheckedState(GameState->bShootSilenced ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
    KillcamCheckbox->SetCheckedState(GameState->bUseKillcam ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);

	MassTextBox->SetText(FText::FromString(FString::Printf(TEXT("%f"), GameState->BulletMassGrams)));
	CaliberTextBox->SetText(FText::FromString(FString::Printf(TEXT("%f"), GameState->BulletCaliberMillimeters)));
    DragCoefficientTextBox->SetText(FText::FromString(FString::Printf(TEXT("%f"), GameState->BulletDragCoefficient)));
    ShotSpeedTextBox->SetText(FText::FromString(FString::Printf(TEXT("%f"), GameState->ShootingSpeedMetersPerSecond)));
    HeightAboveSeaLevelTextBox->SetText(FText::FromString(FString::Printf(TEXT("%f"), GameState->HeightAboveSeaLevelMeters)));
    TemperatureTextBox->SetText(FText::FromString(FString::Printf(TEXT("%f"), GameState->TemperatureCelsius)));
    WindSpeedTextBox->SetText(FText::FromString(FString::Printf(TEXT("%f"), GameState->WindSpeedMetersPerSecond)));
    WindAngleTextBox->SetText(FText::FromString(FString::Printf(TEXT("%f"), GameState->WindAngleDegrees)));
}

void USettingsWidget::NativeDestruct()
{
    ResetBindings();
    Super::NativeDestruct();
}

void USettingsWidget::SetBindings()
{
    SniperPlayer = Cast<ASniperPlayer>(GetOwningPlayerPawn());
    GameState = Cast<ASniperSimulatorGameState>(UGameplayStatics::GetGameState(this));
    PlayerController = UGameplayStatics::GetPlayerController(this, 0);
}

void USettingsWidget::ResetBindings()
{
    PlayerController = nullptr;
    GameState = nullptr;
    SniperPlayer = nullptr;
}