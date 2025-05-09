#pragma once

#define STRF(...) FString::Printf(L ## __VA_ARGS__)
#define LOG(ToLog) UABDebug::Log(this, ToLog, __LINE__, FString(__FUNCTION__))
#define LOGF(...) LOG(STRF(__VA_ARGS__))

#define CLIENT_OR_SERVER_DEBUG FString ClientOrServerDebug = UABDebug::GetClientOrServerDebugString(GetWorld())

class ABUTILS_API UABDebug
{
public:
	static void Log(UObject* WorldContextObject, FString String, int32 Line, FString FunctionName);
	static void Log(UObject* WorldContextObject, const char* String, int32 Line, FString FunctionName);
	static void Log(UObject* WorldContextObject, int32 Value, int32 Line, FString FunctionName);
	static void Log(UObject* WorldContextObject, float Value, int32 Line, FString FunctionName);
	static void Log(UObject* WorldContextObject, bool bValue, int32 Line, FString FunctionName);

	static FString GetClientOrServerDebugString(UWorld* World);
	static FString NetModeToString(ENetMode NetMode);
};
