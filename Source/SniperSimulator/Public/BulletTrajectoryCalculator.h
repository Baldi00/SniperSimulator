#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BulletTrajectoryCalculator.generated.h"

USTRUCT(BlueprintType)
struct FBulletTrajectoryParameters
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BulletMassKg; // [kg]
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BulletDragCoefficient = 0.1128f; // [1]
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BulletArea; // [m^2]
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ShootInitialSpeed = 900; // [m/s]

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AirDensity; // [kg/m^3]

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector InitialPosition; // [m]
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator RifleRotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WindSpeed = 5; // [m/s]
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WindAngle = 90; // [degrees]

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SimulationTimeInterval = 0.02f; // [s]
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SimulationTime = 3; // [s]
};

USTRUCT(BlueprintType)
struct FTrajectoryPointData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Point;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TimeOfFlight;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SuggestedElevationClicks;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SuggestedWindageClicks;
};

UCLASS()
class SNIPERSIMULATOR_API UBulletTrajectoryCalculator : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    static void ComputeTrajectory(TArray<FVector>& OutPositions, const FBulletTrajectoryParameters& Parameters);

    UFUNCTION(BlueprintCallable)
    static FVector GetPositionAtTime(const TArray<FVector>& Positions, const float SimulationTimeInterval, const float Time);

    UFUNCTION(BlueprintCallable)
    static FTrajectoryPointData GetTrajectoryPointDataAtDistance(const TArray<FVector>& Positions, const FVector& InitialPosition, const FRotator& RifleRotation, const float SimulationTimeInterval, const float Distance);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    static bool GetImpactPoint(const UObject* WorldContextObject, const TArray<FVector>& Positions, FVector& ImpactPoint, AActor*& ImpactActor);

    static FHitResult SphereTraceAlongTrajectory(const UObject* WorldContextObject, const TArray<FVector>& Positions, float MaxTargetSpeed, float SimulationIntervalTime, const TArray<FString>& Tags, FVector& TrajectoryImpactLocation);
};
