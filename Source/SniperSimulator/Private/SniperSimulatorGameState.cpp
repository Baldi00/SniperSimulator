#include "SniperSimulatorGameState.h"
#include "Kismet/GameplayStatics.h"
#include "SettingsManager.h"

ASniperSimulatorGameState::ASniperSimulatorGameState()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASniperSimulatorGameState::BeginPlay()
{
    Super::BeginPlay();
    ComputeTrajectoryParameters();
    ComputeShootingTable();

    SettingsManager = Cast<ASettingsManager>(UGameplayStatics::GetActorOfClass(this, ASettingsManager::StaticClass()));
    RandomizeWind();
    UpdateHourOfDay();
    UpdateVisionMode();
}

void ASniperSimulatorGameState::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    WindSpeedNoise = 0.25f * FMath::PerlinNoise1D(UGameplayStatics::GetRealTimeSeconds(this));
    WindAngleNoise = 10 * FMath::PerlinNoise1D(UGameplayStatics::GetRealTimeSeconds(this) + 5);
    OnWindUpdated.Broadcast(WindSpeedMetersPerSecond + WindSpeedNoise, GetWindAngleRelativeToPlayer());
}

float ASniperSimulatorGameState::GetWindAngleRelativeToPlayer()
{
    float PlayerAngle = UGameplayStatics::GetPlayerController(this, 0)->GetControlRotation().Yaw;
    if (PlayerAngle > 180)
        PlayerAngle -= 360;

    return WindAngleDegrees + WindAngleNoise - PlayerAngle;
}

void ASniperSimulatorGameState::ComputeTrajectory(const FVector& InitialPosition, const FRotator& RifleRotation, const float WindSpeed, const float WindAngle)
{
    CurrentTrajectoryParameters.InitialPosition = InitialPosition;
    CurrentTrajectoryParameters.RifleRotation = RifleRotation;
    CurrentTrajectoryParameters.WindSpeed = WindSpeed;
    CurrentTrajectoryParameters.WindAngle = WindAngle;
    UBulletTrajectoryCalculator::ComputeTrajectory(CurrentTrajectory, CurrentTrajectoryParameters);
}

void ASniperSimulatorGameState::ComputeTrajectory(const FVector& InitialPosition, const FRotator& RifleRotation)
{
    CurrentTrajectoryParameters.InitialPosition = InitialPosition;
    CurrentTrajectoryParameters.RifleRotation = RifleRotation;
    CurrentTrajectoryParameters.WindSpeed = WindSpeedMetersPerSecond;
    CurrentTrajectoryParameters.WindAngle = GetWindAngleRelativeToPlayer();
    UBulletTrajectoryCalculator::ComputeTrajectory(CurrentTrajectory, CurrentTrajectoryParameters);
}

void ASniperSimulatorGameState::ComputeImpactPoint()
{
    bIsImpactPointValid = UBulletTrajectoryCalculator::GetImpactPoint(this, CurrentTrajectory, ImpactPoint);
}

void ASniperSimulatorGameState::SaveShootData()
{
    ShootedTrajectory = CurrentTrajectory;
    bIsShootedImpactPointValid = UBulletTrajectoryCalculator::GetImpactPoint(this, ShootedTrajectory, ShootedImpactPoint);
}

void ASniperSimulatorGameState::ComputeTrajectoryParameters()
{
    ComputeBulletParameters();
    ComputeAirDensity();
    SetSimulationParameters();
}

void ASniperSimulatorGameState::ComputeBulletParameters()
{
    CurrentTrajectoryParameters.BulletMassKg = BulletMassGrams / 1000.f;
    CurrentTrajectoryParameters.BulletDragCoefficient = BulletDragCoefficient;
    CurrentTrajectoryParameters.BulletArea = UE_PI * BulletCaliberMillimeters * BulletCaliberMillimeters * 0.00000025f;
    CurrentTrajectoryParameters.ShootInitialSpeed = ShootingSpeedMetersPerSecond;
}

void ASniperSimulatorGameState::ComputeAirDensity()
{
    static const float SeaLevelStandardAtmosphericPressure = 101325.f; // [Pa or J/m^3]
    static const float SeaLevelStandardTemperature = 288.15f; // [K]
    static const float GasConstant = 8.31446261815324f; // [J/(K*mol)]
    static const float MolarMassOfDryAir = 0.0289652f; // [kg/mol]
    static const float CelsiusToKelvin = 273.15f;

    float AirPressure = SeaLevelStandardAtmosphericPressure * FMath::Exp(-9.81f * HeightAboveSeaLevelMeters * MolarMassOfDryAir / (SeaLevelStandardTemperature * GasConstant));
    CurrentTrajectoryParameters.AirDensity = AirPressure * MolarMassOfDryAir / (GasConstant * (TemperatureCelsius + CelsiusToKelvin));
}

void ASniperSimulatorGameState::SetSimulationParameters()
{
    CurrentTrajectoryParameters.SimulationTime = ShotSimulationTimeSeconds;
    CurrentTrajectoryParameters.SimulationTimeInterval = ShotSimulationTimeIntervalSeconds;
}

void ASniperSimulatorGameState::RandomizeWind()
{
    WindSpeedMetersPerSecond = FMath::RandRange(1.f, 15.f);
    WindAngleDegrees = FMath::RandRange(-180.f, 180.f);
}

void ASniperSimulatorGameState::ComputeShootingTable()
{
    TArray<FVector> TrajectoryWind30;
    TArray<FVector> TrajectoryWind60;
    TArray<FVector> TrajectoryWind90;
    ComputeTrajectory(FVector::ZeroVector, FRotator::ZeroRotator, 10, 30);
    TrajectoryWind30 = CurrentTrajectory;
    ComputeTrajectory(FVector::ZeroVector, FRotator::ZeroRotator, 10, 60);
    TrajectoryWind60 = CurrentTrajectory;
    ComputeTrajectory(FVector::ZeroVector, FRotator::ZeroRotator, 10, 90);
    TrajectoryWind90 = CurrentTrajectory;

    ShootingTable.Empty();

    for (int i = ShootingTableMin; i <= ShootingTableMax; i += ShootingTableStepIncrement)
    {
        FShootingTableRow Row{};
        Row.Distance = i;
        FTrajectoryPointData PointData = UBulletTrajectoryCalculator::GetTrajectoryPointDataAtDistance(TrajectoryWind30, FVector::ZeroVector, FRotator::ZeroRotator, ShotSimulationTimeIntervalSeconds, i);
        Row.TimeOfFlight = PointData.TimeOfFlight;
        Row.ElevationClicks = PointData.SuggestedElevationClicks;
        Row.WindageClicks30 = PointData.SuggestedWindageClicks;
        PointData = UBulletTrajectoryCalculator::GetTrajectoryPointDataAtDistance(TrajectoryWind60, FVector::ZeroVector, FRotator::ZeroRotator, ShotSimulationTimeIntervalSeconds, i);
        Row.WindageClicks60 = PointData.SuggestedWindageClicks;
        PointData = UBulletTrajectoryCalculator::GetTrajectoryPointDataAtDistance(TrajectoryWind90, FVector::ZeroVector, FRotator::ZeroRotator, ShotSimulationTimeIntervalSeconds, i);
        Row.WindageClicks90 = PointData.SuggestedWindageClicks;
        ShootingTable.Add(Row);
    }
}

void ASniperSimulatorGameState::UpdateHourOfDay()
{
    SettingsManager->SetHourOfDay(HourOfDay);
}

void ASniperSimulatorGameState::UpdateVisionMode()
{
    SettingsManager->SetVisionMode(VisionMode);
}

void ASniperSimulatorGameState::NextVisionMode()
{
    VisionMode = (EVisionMode)((((uint8)VisionMode) + 1) % (int8)EVisionMode::MAX);
    UpdateVisionMode();
}