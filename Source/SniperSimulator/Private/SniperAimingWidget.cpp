#include "SniperAimingWidget.h"
#include "Components/TextBlock.h"

void USniperAimingWidget::NativeConstruct()
{
    Super::NativeConstruct();
    SetBindings();
}

void USniperAimingWidget::NativeDestruct()
{
    ResetBindings();
    Super::NativeDestruct();
}

void USniperAimingWidget::SetBindings()
{
}

void USniperAimingWidget::ResetBindings()
{
}

void USniperAimingWidget::UpdateTargetDistance(float NewTargetDistance)
{
    TargetDistance->SetText(FText::FromString(FString::Printf(TEXT("%d m"), static_cast<int32>(NewTargetDistance))));
}

void USniperAimingWidget::UpdateWindSpeed(float NewWindSpeed)
{
    WindSpeed->SetText(FText::FromString(FString::Printf(TEXT("%4.1f m/s"), NewWindSpeed)));
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