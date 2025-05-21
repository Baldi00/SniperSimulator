#include "LineDrawerWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "SniperPlayer.h"
#include "SniperSimulatorGameState.h"
#include "ABDebug.h"

void ULineDrawerWidget::NativeDestruct()
{
    SniperPlayer = nullptr;
    GameState = nullptr;
    PlayerController = nullptr;
    Super::NativeDestruct();
}

void ULineDrawerWidget::SetData(ASniperPlayer* InSniperPlayer, ASniperSimulatorGameState* InGameState, APlayerController* InPlayerController)
{
    SniperPlayer = InSniperPlayer;
    GameState = InGameState;
    PlayerController = InPlayerController;
}

int32 ULineDrawerWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    int32 Result = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
    if (GameState == nullptr || PlayerController == nullptr)
        return Result;

    FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
    FVector2D StartScreen;
    FVector2D EndScreen;
    FVector2D StartViewport;
    FVector2D EndViewport;

    if (GameState->IsDrawTrajectoryEnabled())
    {
        TArray<FVector2D> Points;
        for (int i = 1; i < GameState->GetCurrentTrajectory().Num(); i++)
        {
            PlayerController->ProjectWorldLocationToScreen(GameState->GetCurrentTrajectory()[i - 1], StartScreen);
            PlayerController->ProjectWorldLocationToScreen(GameState->GetCurrentTrajectory()[i], EndScreen);

            USlateBlueprintLibrary::ScreenToViewport(PlayerController, StartScreen, StartViewport);
            USlateBlueprintLibrary::ScreenToViewport(PlayerController, EndScreen, EndViewport);

            float DistanceSquared = FVector2D::DistSquared(StartViewport, EndViewport);
            if (DistanceSquared > 0.1 && DistanceSquared < 2500)
                Points.Add(StartViewport);
        }
        UWidgetBlueprintLibrary::DrawLines(Context, Points, FLinearColor::Red);
    }

    if (SniperPlayer != nullptr && SniperPlayer->GetSpawnedBulletActor() != nullptr)
    {
        if (PlayerController->ProjectWorldLocationToScreen(SniperPlayer->GetSpawnedBulletActor()->GetActorLocation(), StartScreen))
        {
            USlateBlueprintLibrary::ScreenToViewport(PlayerController, StartScreen, StartViewport);
            UWidgetBlueprintLibrary::DrawLine(Context, StartViewport, StartViewport - FVector2D(0, 3), FLinearColor::White, true, 3);
        }
    }

    return Result;
}