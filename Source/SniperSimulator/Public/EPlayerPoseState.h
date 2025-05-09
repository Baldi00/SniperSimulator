#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EPlayerPoseState : uint8
{
    STANDING,
    CROUCHED,
    PRONE
};