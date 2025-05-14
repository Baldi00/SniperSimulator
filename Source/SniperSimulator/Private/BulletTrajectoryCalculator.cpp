#include "BulletTrajectoryCalculator.h"
#include "ABDebug.h"

void UBulletTrajectoryCalculator::ComputeTrajectory(TArray<FVector>& OutPositions, FBulletTrajectoryParameters Parameters)
{
    TArray<FVector> Res;

    //OutPositions.Empty();
    //OutPositions.SetNum(Parameters.SimulationTime / Parameters.SimulationTimeInterval + 1);

    FVector InitialPosition = FVector::ZeroVector;
    FVector CurrentPosition = InitialPosition;
    FVector CurrentVelocity = FVector::ForwardVector * Parameters.ShootInitialSpeed;

    int32 CurrentComputedPosition = 0;
    for (float CurrentTime = 0; CurrentTime < Parameters.SimulationTime; CurrentTime += Parameters.SimulationTimeInterval)
    {
        //if (FMath::Fmod(CurrentPosition.X, 100) > 0.5f && FMath::Fmod(CurrentPosition.X, 100) < 1.5f)
            //UABDebug::Log(GEngine->GetWorld(), CurrentPosition.ToString(), __LINE__, FString(__FUNCTION__));

        // Result
        Res.Add(Parameters.InitialPosition + Parameters.RifleRotation.RotateVector(CurrentPosition * 100));
        CurrentComputedPosition++;

        float Speed = CurrentVelocity.Length();

        // Air drag
        FVector DragForce = -0.5f * Parameters.BulletDragCoefficient * Parameters.AirDensity * Parameters.BulletArea * Speed * Speed * CurrentVelocity.GetSafeNormal();

        // Wind
        FVector WindVelocity(FMath::Abs(Parameters.WindSpeed) * FMath::Cos(FMath::DegreesToRadians(Parameters.WindSpeed)), FMath::Abs(Parameters.WindSpeed) * FMath::Sin(FMath::DegreesToRadians(Parameters.WindSpeed)), 0);
        FVector RelativeWind = WindVelocity - CurrentVelocity;
        FVector WindForce = 0.5f * Parameters.BulletDragCoefficient * Parameters.AirDensity * Parameters.BulletArea * RelativeWind.SquaredLength() * RelativeWind.GetSafeNormal();

        // Gravity
        FVector GravityForce = FVector(0, 0, -9.81f) * Parameters.BulletMassKg;

        // Total force
        FVector TotalForce = DragForce + WindForce + GravityForce;

        // Integration
        CurrentPosition += CurrentVelocity * Parameters.SimulationTimeInterval;
        CurrentVelocity += TotalForce / Parameters.BulletMassKg * Parameters.SimulationTimeInterval;

        // Windage (Didion)
        float Deviation = WindVelocity.Length() * (CurrentTime - CurrentPosition.X / Parameters.ShootInitialSpeed);
        Deviation *= FMath::Sin(FMath::DegreesToRadians(Parameters.WindAngle));
        CurrentPosition.Y = Deviation;
    }

    OutPositions = Res;
}