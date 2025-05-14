#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BulletTrajectoryCalculator.generated.h"

USTRUCT(BlueprintType)
struct FBulletTrajectoryParameters
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BulletMass = 48.6f; // [g]
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BulletMassKg; // [kg]
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BulletCaliber = 12.7f; // [mm]
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BulletDragCoefficient = 0.1128f; // [1]
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BulletArea; // [m^2]

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HeightAboveSeaLevel = 0; // [m]
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Temperature = 15; // [°C]
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AirDensity; // [kg/m^3]

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector InitialPosition; // [m]
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator RifleRotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ShootInitialSpeed = 900; // [m/s]
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector ShootDirection; // [m]

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WindSpeed = 5; // [m/s]
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WindAngle = 90; // [degrees]

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SimulationTimeInterval = 0.02f; // [s]
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SimulationTime = 3; // [s]
};

UCLASS()
class SNIPERSIMULATOR_API UBulletTrajectoryCalculator : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    static void ComputeTrajectory(TArray<FVector>& OutPositions, FBulletTrajectoryParameters Parameters);
};
