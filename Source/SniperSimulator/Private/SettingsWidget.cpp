#include "SettingsWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Kismet/GameplayStatics.h"
#include "SniperSimulatorGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "SniperPlayer.h"
#include "Misc/DefaultValueHelper.h"

void USettingsWidget::NativeConstruct()
{
    Super::NativeConstruct();
    SetBindings();
    PopulateUI();
    UGameplayStatics::SetGamePaused(this, true);
    UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PlayerController);
    PlayerController->bShowMouseCursor = true;
}

void USettingsWidget::NativeDestruct()
{
    UGameplayStatics::SetGamePaused(this, false);
    UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
    PlayerController->bShowMouseCursor = false;
    ResetBindings();
    Super::NativeDestruct();
}

void USettingsWidget::SetBindings()
{
    SniperPlayer = Cast<ASniperPlayer>(GetOwningPlayerPawn());
    GameState = Cast<ASniperSimulatorGameState>(UGameplayStatics::GetGameState(this));
    PlayerController = UGameplayStatics::GetPlayerController(this, 0);

    RandomizeWindButton->OnClicked.AddUniqueDynamic(this, &USettingsWidget::RandomizeWind);
    SaveButton->OnClicked.AddUniqueDynamic(this, &USettingsWidget::SaveAndClose);
    ResetButton->OnClicked.AddUniqueDynamic(this, &USettingsWidget::ResetSettings);
    QuitGameButton->OnClicked.AddUniqueDynamic(this, &USettingsWidget::QuitGame);
}

void USettingsWidget::ResetBindings()
{
    QuitGameButton->OnClicked.RemoveDynamic(this, &USettingsWidget::QuitGame);
    ResetButton->OnClicked.RemoveDynamic(this, &USettingsWidget::ResetSettings);
    SaveButton->OnClicked.RemoveDynamic(this, &USettingsWidget::SaveAndClose);
    RandomizeWindButton->OnClicked.RemoveDynamic(this, &USettingsWidget::RandomizeWind);

    PlayerController = nullptr;
    GameState = nullptr;
    SniperPlayer = nullptr;
}

void USettingsWidget::PopulateUI()
{
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

void USettingsWidget::RandomizeWind()
{
    float WindSpeed = FMath::RandRange(1.f, 15.f);
    float WindAngle = FMath::RandRange(-180.f, 180.f);

    WindSpeedTextBox->SetText(FText::FromString(FString::Printf(TEXT("%f"), WindSpeed)));
    WindAngleTextBox->SetText(FText::FromString(FString::Printf(TEXT("%f"), WindAngle)));
}

void USettingsWidget::SaveAndClose()
{
    GameState->bDrawHitPointEnabled = ShowImpactPointCheckbox->GetCheckedState() == ECheckBoxState::Checked;
    GameState->bDrawTrajectoryEnabled = ShowTrajectoryCheckbox->GetCheckedState() == ECheckBoxState::Checked;
    GameState->bShootSilenced = UseSilencerCheckbox->GetCheckedState() == ECheckBoxState::Checked;
    GameState->bUseKillcam = KillcamCheckbox->GetCheckedState() == ECheckBoxState::Checked;
    FDefaultValueHelper::ParseFloat(MassTextBox->GetText().ToString(), GameState->BulletMassGrams);
    FDefaultValueHelper::ParseFloat(CaliberTextBox->GetText().ToString(), GameState->BulletCaliberMillimeters);
    FDefaultValueHelper::ParseFloat(DragCoefficientTextBox->GetText().ToString(), GameState->BulletDragCoefficient);
    FDefaultValueHelper::ParseFloat(ShotSpeedTextBox->GetText().ToString(), GameState->ShootingSpeedMetersPerSecond);
    FDefaultValueHelper::ParseFloat(HeightAboveSeaLevelTextBox->GetText().ToString(), GameState->HeightAboveSeaLevelMeters);
    FDefaultValueHelper::ParseFloat(TemperatureTextBox->GetText().ToString(), GameState->TemperatureCelsius);
    FDefaultValueHelper::ParseFloat(WindSpeedTextBox->GetText().ToString(), GameState->WindSpeedMetersPerSecond);
    FDefaultValueHelper::ParseFloat(WindAngleTextBox->GetText().ToString(), GameState->WindAngleDegrees);
    GameState->ComputeTrajectoryParameters();
    GameState->ComputeShootingTable();
    CloseSettings();
}

void USettingsWidget::ResetSettings()
{
    GameState->bDrawHitPointEnabled = true;
    GameState->bDrawTrajectoryEnabled = true;
    GameState->bShootSilenced = true;
    GameState->bUseKillcam = false;
    GameState->BulletMassGrams = 48.6f;
    GameState->BulletCaliberMillimeters = 12.7f;
    GameState->BulletDragCoefficient = 0.1128f;
    GameState->ShootingSpeedMetersPerSecond = 900;
    GameState->HeightAboveSeaLevelMeters = 0.f;
    GameState->TemperatureCelsius = 15.f;
    GameState->WindSpeedMetersPerSecond = 5.f;
    GameState->WindAngleDegrees = 0.f;
    GameState->ComputeTrajectoryParameters();
    GameState->ComputeShootingTable();
    PopulateUI();
}

void USettingsWidget::CloseSettings()
{
    RemoveFromParent();
}

void USettingsWidget::QuitGame()
{
    UKismetSystemLibrary::QuitGame(this, PlayerController, EQuitPreference::Quit, true);
}