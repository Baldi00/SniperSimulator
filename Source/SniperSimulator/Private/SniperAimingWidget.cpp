#include "SniperAimingWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "SniperSimulatorGameState.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "SniperPlayer.h"
#include "Components/CanvasPanelSlot.h"

void USniperAimingWidget::NativeConstruct()
{
    Super::NativeConstruct();
    SetBindings();
    UpdateZoomLevel(SniperPlayer->GetCurrentZoomLevel());
    UpdateTargetDistance(SniperPlayer->GetCurrentTargetDistance());
    UpdateCurrentWind(GameState->GetWindSpeed(), GameState->GetWindAngle());
    UpdateElevation(SniperPlayer->GetCurrentElevationLevel());
    UpdateWindCorrection(SniperPlayer->GetCurrentWindageLevel());
}

void USniperAimingWidget::NativeDestruct()
{
    ResetBindings();
    Super::NativeDestruct();
}

void USniperAimingWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    if (GameState == nullptr || PlayerController == nullptr || !GameState->IsImpactPointValid())
    {
        ImpactPointImage->SetVisibility(ESlateVisibility::Hidden);
        return;
    }
    
    ImpactPointImage->SetVisibility(ESlateVisibility::Visible);

    FVector2D ImpactPointScreen;
    FVector2D ImpactPointViewport;

    if (PlayerController->ProjectWorldLocationToScreen(GameState->GetImpactPoint(), ImpactPointScreen))
    {
        USlateBlueprintLibrary::ScreenToViewport(PlayerController, ImpactPointScreen, ImpactPointViewport);
        UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ImpactPointImage->Slot);
        if (CanvasSlot)
            CanvasSlot->SetPosition(ImpactPointViewport);
    }
}

int32 USniperAimingWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    int32 Result = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
    if (GameState == nullptr || PlayerController == nullptr)
        return Result;

    FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
    FVector2D StartScreen;
    FVector2D EndScreen;
    FVector2D StartViewport;
    FVector2D EndViewport;

    UWidgetBlueprintLibrary::DrawLine(Context, StartViewport, EndViewport, FLinearColor::Red, true, 3.f);

    for (int i = 1; i < GameState->GetCurrentTrajectory().Num(); i++)
    {
        bool Success1 = PlayerController->ProjectWorldLocationToScreen(GameState->GetCurrentTrajectory()[i - 1], StartScreen);
        bool Success2 = PlayerController->ProjectWorldLocationToScreen(GameState->GetCurrentTrajectory()[i], EndScreen);
        if (!Success1 || !Success2)
            continue;
        USlateBlueprintLibrary::ScreenToViewport(PlayerController, StartScreen, StartViewport);
        USlateBlueprintLibrary::ScreenToViewport(PlayerController, EndScreen, EndViewport);
        float DistanceSquared = FVector2D::DistSquared(StartViewport, EndViewport);
        if (DistanceSquared > 0.1 && DistanceSquared < 2500)
            UWidgetBlueprintLibrary::DrawLine(Context, StartViewport, EndViewport, FLinearColor(0.25f, 0.25f, 0.25f, 0.5f));
    }

    return Result;
}

void USniperAimingWidget::SetBindings()
{
    SniperPlayer = Cast<ASniperPlayer>(GetOwningPlayerPawn());
    GameState = Cast<ASniperSimulatorGameState>(UGameplayStatics::GetGameState(this));
    PlayerController = UGameplayStatics::GetPlayerController(this, 0);

    SniperPlayer->OnZoomLevelUpdated.AddUniqueDynamic(this, &USniperAimingWidget::UpdateZoomLevel);
    SniperPlayer->OnTargetDistandceUpdated.AddUniqueDynamic(this, &USniperAimingWidget::UpdateTargetDistance);
    SniperPlayer->OnElevationRegulationUpdated.AddUniqueDynamic(this, &USniperAimingWidget::UpdateElevation);
    SniperPlayer->OnWindageRegulationUpdated.AddUniqueDynamic(this, &USniperAimingWidget::UpdateWindCorrection);
    GameState->OnWindUpdated.AddUniqueDynamic(this, &USniperAimingWidget::UpdateCurrentWind);

}

void USniperAimingWidget::ResetBindings()
{
    GameState->OnWindUpdated.RemoveDynamic(this, &USniperAimingWidget::UpdateCurrentWind);
    SniperPlayer->OnWindageRegulationUpdated.RemoveDynamic(this, &USniperAimingWidget::UpdateWindCorrection);
    SniperPlayer->OnElevationRegulationUpdated.RemoveDynamic(this, &USniperAimingWidget::UpdateElevation);
    SniperPlayer->OnTargetDistandceUpdated.RemoveDynamic(this, &USniperAimingWidget::UpdateTargetDistance);
    SniperPlayer->OnZoomLevelUpdated.RemoveDynamic(this, &USniperAimingWidget::UpdateZoomLevel);

    PlayerController = nullptr;
    GameState = nullptr;
    SniperPlayer = nullptr;
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

void USniperAimingWidget::UpdateElevation(int32 NewElevation)
{
    if (NewElevation > 0)
        Elevation->SetText(FText::FromString(FString::Printf(TEXT("%d down clicks"), FMath::Abs(NewElevation))));
    else if (NewElevation < 0)
        Elevation->SetText(FText::FromString(FString::Printf(TEXT("%d up clicks"), FMath::Abs(NewElevation))));
    else
        Elevation->SetText(FText::FromString(TEXT("0 clicks")));
}

void USniperAimingWidget::UpdateWindCorrection(int32 NewWindCorrection)
{
    if (NewWindCorrection > 0)
        WindCorrection->SetText(FText::FromString(FString::Printf(TEXT("%d left clicks"), FMath::Abs(NewWindCorrection))));
    else if (NewWindCorrection < 0)
        WindCorrection->SetText(FText::FromString(FString::Printf(TEXT("%d right clicks"), FMath::Abs(NewWindCorrection))));
    else
        WindCorrection->SetText(FText::FromString(TEXT("0 clicks")));
}