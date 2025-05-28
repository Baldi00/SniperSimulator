#include "BulletTrajectoryCalculator.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ABMath.h"

void UBulletTrajectoryCalculator::ComputeTrajectory(TArray<FVector>& OutPositions, const FBulletTrajectoryParameters& Parameters)
{
    OutPositions.SetNum(Parameters.SimulationTime / Parameters.SimulationTimeInterval + 1);

    FVector InitialPosition = FVector::ZeroVector;
    FVector CurrentPosition = InitialPosition;
    FVector CurrentVelocity = FVector::ForwardVector * Parameters.ShootInitialSpeed;

    int32 CurrentComputedPosition = 0;
    for (float CurrentTime = 0; CurrentTime < Parameters.SimulationTime; CurrentTime += Parameters.SimulationTimeInterval)
    {
        // Result
        OutPositions[CurrentComputedPosition] = Parameters.InitialPosition + Parameters.RifleRotation.RotateVector(CurrentPosition * 100);
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
        if (Deviation < 0)
            Deviation = 0;
        else
            Deviation *= FMath::Sin(FMath::DegreesToRadians(Parameters.WindAngle));
        CurrentPosition.Y = Deviation;
    }
}

FVector UBulletTrajectoryCalculator::GetPositionAtTime(const TArray<FVector>& Positions, const float SimulationTimeInterval, const float Time)
{
    float PositionIndex = Time / SimulationTimeInterval;
    if (FMath::FloorToInt(PositionIndex) == 0)
        return FMath::Lerp(Positions[0], Positions[1], PositionIndex);
    return FMath::Lerp(Positions[FMath::FloorToInt(PositionIndex)], Positions[FMath::CeilToInt(PositionIndex)], FMath::Fmod(PositionIndex, FMath::FloorToInt(PositionIndex)));
}

FTrajectoryPointData UBulletTrajectoryCalculator::GetTrajectoryPointDataAtDistance(const TArray<FVector>& Positions, const FVector& InitialPosition, const FRotator& RifleRotation, const float SimulationTimeInterval, const float Distance)
{
    for (int i = 1; i < Positions.Num(); i++)
    {
        float DistanceSquared1 = FVector::DistSquared(InitialPosition, Positions[i - 1]);
        float DistanceSquared2 = FVector::DistSquared(InitialPosition, Positions[i]);
        if (DistanceSquared1 <= Distance * Distance && DistanceSquared2 >= Distance * Distance)
        {
            FTrajectoryPointData Result{};
            float Interpolator = UABMath::InverseLerp(FMath::Sqrt(DistanceSquared1), FMath::Sqrt(DistanceSquared2), Distance);
            Result.Point = FMath::Lerp(Positions[i - 1], Positions[i], Interpolator);
            Result.TimeOfFlight = FMath::Lerp(SimulationTimeInterval * (i - 1), SimulationTimeInterval * i, Interpolator);

            if (Distance <= 0)
                Result.SuggestedElevationClicks = 0;
            else
                Result.SuggestedElevationClicks = FMath::RoundToInt(FMath::RadiansToDegrees(FMath::Atan((InitialPosition.Z - Result.Point.Z) / Distance)) / 0.0057f); // Atan(0.01/100) * RadToDeg = 0.0057f, base click for 1cm at 100m

            FVector ShootForwardVector = FRotationMatrix(RifleRotation).GetScaledAxis(EAxis::X);

            FVector CurrentPointDifference = Result.Point - InitialPosition;
            CurrentPointDifference.Z = InitialPosition.Z;
            CurrentPointDifference = CurrentPointDifference.GetSafeNormal() * (CurrentPointDifference.Length() + 10000); // Correction of 100m, I don't know why it's needed
            ShootForwardVector *= CurrentPointDifference.Length();

            float Deviation = 0;
            if (Distance > 20000) // Wrong numbers at short distances
                Deviation = CurrentPointDifference.Length() * FMath::Sin(FMath::DegreesToRadians(UABMath::AngleBetweenVectors(ShootForwardVector, CurrentPointDifference)));

            if (Distance <= 0)
                Result.SuggestedWindageClicks = 0;
            else
                Result.SuggestedWindageClicks = FMath::RoundToInt(FMath::RadiansToDegrees(FMath::Atan(Deviation / Distance)) / 0.0057f);

            return Result;
        }
    }
    return FTrajectoryPointData();
}

bool UBulletTrajectoryCalculator::GetImpactPoint(const UObject* WorldContextObject, const TArray<FVector>& Positions, FVector& ImpactPoint, AActor*& ImpactActor)
{
    TArray<AActor*> Ignored;
    FHitResult HitResult;
    for (int i = 1; i < Positions.Num(); i++)
    {
        if (UKismetSystemLibrary::LineTraceSingle(WorldContextObject, Positions[i - 1], Positions[i], ETraceTypeQuery::TraceTypeQuery1, false, Ignored, EDrawDebugTrace::None, HitResult, true))
        {
            ImpactPoint = HitResult.ImpactPoint;
            ImpactActor = HitResult.GetActor();
            return true;
        }
    }
    return false;
}

FHitResult UBulletTrajectoryCalculator::SphereTraceAlongTrajectory(const UObject* WorldContextObject, const TArray<FVector>& Positions, float MaxTargetSeed, float SimulationIntervalTime, const TArray<FString>& Tags, FVector& TrajectoryImpactLocation)
{
    TArray<FHitResult> HitResults;
    for (int i = 1; i < Positions.Num(); i++)
    {
        float SphereRadius = (i - 1) * SimulationIntervalTime * MaxTargetSeed;
        if (UKismetSystemLibrary::SphereTraceMulti(WorldContextObject, Positions[i - 1], Positions[i], SphereRadius, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::None, HitResults, true))
            if (Tags.Num() > 0)
            {
                for (FHitResult& HitResult : HitResults)
                    for (const FString& Tag : Tags)
                        if (HitResult.GetActor()->ActorHasTag(FName(*Tag)))
                        {
                            TrajectoryImpactLocation = FMath::Lerp(Positions[i - 1], Positions[i], HitResult.Distance / FVector::Dist(Positions[i - 1], Positions[i]));
                            return HitResult;
                        }
            }
            else
            {
                TrajectoryImpactLocation = FMath::Lerp(Positions[i - 1], Positions[i], HitResults[0].Distance / FVector::Dist(Positions[i - 1], Positions[i]));
                return HitResults[0];
            }
    }
    return FHitResult();
}
