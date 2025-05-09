#include "ABMath.h"

float UABMath::AngleBetweenVectors(const FVector& InVector1, const FVector& InVector2)
{
	return FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(InVector1.GetSafeNormal(), InVector2.GetSafeNormal())));
}

float UABMath::InverseLerp(float min, float max, float val)
{
	return (val - min) / (max - min);
}

float UABMath::Remap(float iMin, float iMax, float oMin, float oMax, float val)
{
	return FMath::Lerp(oMin, oMax, InverseLerp(iMin, iMax, val));
}

FVector UABMath::ComputeBulletInitialVelocity(const FVector& SpawnLocation, const FVector& TargetLocation, float TimeToReach)
{
	const FVector SpawnToTargetVector = TargetLocation - SpawnLocation;
	TimeToReach = FMath::Max(TimeToReach, 0.01);
	const float XVelocity0 = SpawnToTargetVector.X / TimeToReach;
	const float YVelocity0 = SpawnToTargetVector.Y / TimeToReach;
	const float ZVelocity0 = (SpawnToTargetVector.Z + 0.5f * 980 * TimeToReach * TimeToReach) / TimeToReach;

	return { XVelocity0, YVelocity0, ZVelocity0 };
}
