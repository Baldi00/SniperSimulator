#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BulletTrajectoryCalculator.h"
#include "SniperSimulatorGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWindUpdated, float, InCurrentWindSpeed, float, InCurrentWindAngle);

USTRUCT(BlueprintType)
struct FShootingTableRow
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float Distance;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float TimeOfFlight;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 ElevationClicks;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 WindageClicks30;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 WindageClicks60;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 WindageClicks90;
};

UENUM(BlueprintType)
enum class EHourOfDay : uint8
{
    DAY,
    NIGHT
};

UENUM(BlueprintType)
enum class EVisionMode : uint8
{
    DEFAULT,
    NIGHT,
    TACTICAL
};

class ASettingsManager;

UCLASS()
class SNIPERSIMULATOR_API ASniperSimulatorGameState : public AGameStateBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ShotSimulation, meta = (AllowPrivateAccess = "true"))
    float ShotSimulationTimeSeconds = 4;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ShotSimulation, meta = (AllowPrivateAccess = "true"))
    float ShotSimulationTimeIntervalSeconds = 0.02f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ShotSimulation, meta = (AllowPrivateAccess = "true"))
    int32 ShootingTableMin = 0;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ShotSimulation, meta = (AllowPrivateAccess = "true"))
    int32 ShootingTableMax = 230000;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ShotSimulation, meta = (AllowPrivateAccess = "true"))
    int32 ShootingTableStepIncrement = 10000;

    FBulletTrajectoryParameters CurrentTrajectoryParameters;
    UPROPERTY(VisibleAnywhere)
    TArray<FShootingTableRow> ShootingTable;
    TArray<FVector> CurrentTrajectory;
    TArray<FVector> ShootedTrajectory;
    FVector ImpactPoint;
    FVector ShootedImpactPoint;
    bool bIsImpactPointValid = false;
    bool bIsShootedImpactPointValid = false;

    float WindSpeedNoise = 0;
    float WindAngleNoise = 0;

    TObjectPtr<ASettingsManager> SettingsManager;

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Bullet, meta = (AllowPrivateAccess = "true"))
    float BulletMassGrams = 48.6f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Bullet, meta = (AllowPrivateAccess = "true"))
    float BulletCaliberMillimeters = 12.7f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Bullet, meta = (AllowPrivateAccess = "true"))
    float BulletDragCoefficient = 0.1128f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Rifle, meta = (AllowPrivateAccess = "true"))
    float ShootingSpeedMetersPerSecond = 900;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Atmosphere, meta = (AllowPrivateAccess = "true"))
    float HeightAboveSeaLevelMeters = 0;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Atmosphere, meta = (AllowPrivateAccess = "true"))
    float TemperatureCelsius = 15;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Wind, meta = (AllowPrivateAccess = "true"))
    float WindSpeedMetersPerSecond = 5;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Wind, meta = (AllowPrivateAccess = "true"))
    float WindAngleDegrees = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = OtherSettings, meta = (AllowPrivateAccess = "true"))
    bool bUseKillcam = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = OtherSettings, meta = (AllowPrivateAccess = "true"))
    bool bShootSilenced = true;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = OtherSettings, meta = (AllowPrivateAccess = "true"))
    bool bDrawTrajectoryEnabled = true;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = OtherSettings, meta = (AllowPrivateAccess = "true"))
    bool bDrawHitPointEnabled = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = OtherSettings, meta = (AllowPrivateAccess = "true"))
    EHourOfDay HourOfDay = EHourOfDay::DAY;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = OtherSettings, meta = (AllowPrivateAccess = "true"))
    EVisionMode VisionMode = EVisionMode::DEFAULT;

    ASniperSimulatorGameState();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    FORCEINLINE float GetWindSpeed() const { return WindSpeedMetersPerSecond + WindSpeedNoise; }
    FORCEINLINE float GetWindAngle() const { return WindAngleDegrees + WindAngleNoise; }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetWindAngleRelativeToPlayer();

    FOnWindUpdated OnWindUpdated;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    void ComputeTrajectory(const FVector& InitialPosition, const FRotator& RifleRotation, const float WindSpeed, const float WindAngle);

    void ComputeTrajectory(const FVector& InitialPosition, const FRotator& RifleRotation);

    void ComputeImpactPoint();

    UFUNCTION(BlueprintCallable)
    FORCEINLINE TArray<FShootingTableRow>& GetShootingTable() { return ShootingTable; }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    FORCEINLINE TArray<FVector>& GetCurrentTrajectory() { return CurrentTrajectory; }
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FORCEINLINE TArray<FVector>& GetShootedTrajectory() { return ShootedTrajectory; }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE FVector& GetImpactPoint() { return ImpactPoint; }
    UFUNCTION(BlueprintCallable)
    FORCEINLINE bool IsImpactPointValid() const { return bIsImpactPointValid; }
    UFUNCTION(BlueprintCallable)
    FORCEINLINE FVector& GetShootedImpactPoint() { return ShootedImpactPoint; }
    UFUNCTION(BlueprintCallable)
    FORCEINLINE bool IsShootedImpactPointValid() const { return bIsShootedImpactPointValid; }

    void SaveShootData();

    FORCEINLINE float GetShotSimulationTimeIntervalSeconds() const { return ShotSimulationTimeIntervalSeconds; }
    FORCEINLINE float GetShotSimulationTimeSeconds() const { return ShotSimulationTimeSeconds; }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE bool GetUseKillcam() const { return bUseKillcam; }
    UFUNCTION(BlueprintCallable)
    FORCEINLINE bool IsShootSilenced() const { return bUseKillcam; }
    UFUNCTION(BlueprintCallable)
    FORCEINLINE bool IsDrawTrajectoryEnabled() const { return bDrawTrajectoryEnabled; }
    UFUNCTION(BlueprintCallable)
    FORCEINLINE bool IsDrawHitPointEnabled() const { return bDrawHitPointEnabled; }

    void ComputeTrajectoryParameters();
    void ComputeShootingTable();
    void UpdateHourOfDay();
    void UpdateVisionMode();

private:
    void ComputeBulletParameters();
    void ComputeAirDensity();
    void SetSimulationParameters();
    void RandomizeWind();
};
