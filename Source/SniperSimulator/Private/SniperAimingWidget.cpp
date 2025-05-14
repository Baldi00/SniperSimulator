#include "SniperAimingWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "SniperSimulatorGameState.h"
#include "SniperPlayer.h"

void USniperAimingWidget::NativeConstruct()
{
    Super::NativeConstruct();
    SetBindings();
    UpdateZoomLevel(SniperPlayer->GetCurrentZoomLevel());
    UpdateTargetDistance(SniperPlayer->GetCurrentTargetDistance());
    UpdateCurrentWind(GameState->GetWindSpeed(), GameState->GetWindAngle());
}

void USniperAimingWidget::NativeDestruct()
{
    ResetBindings();
    Super::NativeDestruct();
}

void USniperAimingWidget::SetBindings()
{
    SniperPlayer = Cast<ASniperPlayer>(GetOwningPlayerPawn());
    SniperPlayer->OnZoomLevelUpdated.AddUniqueDynamic(this, &USniperAimingWidget::UpdateZoomLevel);
    SniperPlayer->OnTargetDistandceUpdated.AddUniqueDynamic(this, &USniperAimingWidget::UpdateTargetDistance);

    GameState = Cast<ASniperSimulatorGameState>(UGameplayStatics::GetGameState(this));
    GameState->OnWindUpdated.AddUniqueDynamic(this, &USniperAimingWidget::UpdateCurrentWind);
}

void USniperAimingWidget::ResetBindings()
{
    SniperPlayer->OnZoomLevelUpdated.RemoveDynamic(this, &USniperAimingWidget::UpdateZoomLevel);
    SniperPlayer->OnTargetDistandceUpdated.RemoveDynamic(this, &USniperAimingWidget::UpdateTargetDistance);
    GameState->OnWindUpdated.RemoveDynamic(this, &USniperAimingWidget::UpdateCurrentWind);
}

void USniperAimingWidget::UpdateTargetDistance(float NewTargetDistance)
{
    TargetDistance->SetText(FText::FromString(FString::Printf(TEXT("%4.2f m"), NewTargetDistance)));
}

void USniperAimingWidget::UpdateCurrentWind(float NewWindSpeed, float NewWindAngle)
{
    char WindFrontBackDirection = 'B';
    if (FMath::Abs(NewWindAngle) > 90)
    {
        NewWindAngle = (180 - FMath::Abs(NewWindAngle)) * FMath::Sign(NewWindAngle);
        WindFrontBackDirection = 'F';
    }
    WindSpeed->SetText(FText::FromString(FString::Printf(TEXT("%4.1f m/s | %2.1f° | %c%c"), NewWindSpeed, FMath::Abs(NewWindAngle), NewWindAngle > 0 ? 'L' : 'R', WindFrontBackDirection)));
}

void USniperAimingWidget::UpdateZoomLevel(int32 NewZoomLevel)
{
    ZoomLevel->SetText(FText::FromString(FString::Printf(TEXT("%dx"), NewZoomLevel)));
}

void USniperAimingWidget::UpdateElevation(float NewElevation)
{
    Elevation->SetText(FText::FromString(FString::Printf(TEXT("%d m"), static_cast<int32>(NewElevation))));
}

void USniperAimingWidget::UpdateWindCorrection(float NewWindCorrection)
{
    WindCorrection->SetText(FText::FromString(FString::Printf(TEXT("%4.1f m/s"), NewWindCorrection)));
}