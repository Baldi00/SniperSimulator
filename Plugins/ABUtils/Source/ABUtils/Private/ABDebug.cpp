#include "ABDebug.h"

void UABDebug::Log(UObject* WorldContextObject, FString String, int32 Line, FString FunctionName)
{
    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
    FString ClientOrServerDebugString;
    if (World && World->WorldType == EWorldType::PIE && World->GetNetMode() != NM_Standalone)
        ClientOrServerDebugString = GetClientOrServerDebugString(World) + ": ";

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST) || USE_LOGGING_IN_SHIPPING
    GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Cyan, ClientOrServerDebugString + String);
#endif

    FString ToPrintLogger = ClientOrServerDebugString + FunctionName + "(" + FString::FromInt(Line) + "): " + String;
    UE_LOG(LogTemp, Warning, TEXT("%s"), *ToPrintLogger);
}

void UABDebug::Log(UObject* WorldContextObject, const char* String, int32 Line, FString FunctionName)
{
    Log(WorldContextObject, FString(String), Line, FunctionName);
}

void UABDebug::Log(UObject* WorldContextObject, int32 Value, int32 Line, FString FunctionName)
{
    Log(WorldContextObject, FString::FromInt(Value), Line, FunctionName);
}

void UABDebug::Log(UObject* WorldContextObject, float Value, int32 Line, FString FunctionName)
{
    Log(WorldContextObject, FString::SanitizeFloat(Value), Line, FunctionName);
}

void UABDebug::Log(UObject* WorldContextObject, bool bValue, int32 Line, FString FunctionName)
{
    Log(WorldContextObject, bValue ? FString("true") : FString("false"), Line, FunctionName);
}

FString UABDebug::GetClientOrServerDebugString(UWorld* World)
{
    FString NetModeString = NetModeToString(World->GetNetMode());
    return GPlayInEditorID == 0 ? NetModeString : NetModeString + " " + FString::FromInt(GPlayInEditorID);
}

FString UABDebug::NetModeToString(ENetMode NetMode)
{
    switch (NetMode)
    {
    case NM_Standalone:
        return "Standalone";
        break;
    case NM_DedicatedServer:
        return "Dedicated server";
        break;
    case NM_ListenServer:
        return "Server";
        break;
    case NM_Client:
        return "Client";
        break;
    case NM_MAX:
        break;
    default:
        break;
    }
    return "Error";
}
