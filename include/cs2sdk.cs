using System;
using System.Runtime.CompilerServices;
using Microsoft.Win32.SafeHandles;
using Plugify;

//generated with https://github.com/untrustedmodders/csharp-lang-module/blob/main/generator/generator.py from cs2sdk 

namespace cs2sdk
{
	public enum ResultType
	{
		Continue = 0,
		Changed = 1,
		Handled = 3,
		Stop = 4,
	};
	
	delegate ResultType CommandCallback(int slot, int context, string[] arguments);
	delegate void ChangeCallback(IntPtr pConVar, string newValue, string oldValue);
	delegate void TaskCallback();
	delegate ResultType HookEntityOutputCallback(int activator, int caller, float delay);
	delegate ResultType EventCallback(string name, IntPtr eventInfo, bool dontBroadcast);
	delegate void TimerCallback(IntPtr timer);

	internal static class cs2sdk
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern ulong GetSteamAccountId(int clientIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern string GetClientIp(int clientIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern string GetClientName(int clientIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern float GetClientTime(int clientIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern float GetClientLatency(int clientIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetClientUserId(int clientIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern bool IsClientAuthorized(int clientIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern bool IsClientConnected(int clientIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern bool IsClientInGame(int clientIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern bool IsFakeClient(int clientIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern bool IsPlayerAlive(int clientIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetClientTeam(int clientIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetClientHealth(int clientIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void GetClientAbsOrigin(ref Vector3 output, int clientIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void GetClientAbsAngles(ref Vector3 output, int clientIndex);
		
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void AddCommand(string name, string description, long flags, CommandCallback callback);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void RemoveCommand(string name, CommandCallback callback);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void AddCommandListener(string name, CommandCallback callback, bool post);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void RemoveCommandListener(string name, CommandCallback callback, bool post);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void ServerCommand(string command);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void ClientCommand(int clientIndex, string command);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void ClientCommandFromServer(int clientIndex, string command);
		
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void PrintServer(string msg);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void PrintConsole(int clientIndex, string message);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void PrintChat(int clientIndex, string message);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void PrintCentre(int clientIndex, string message);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void PrintAlert(int clientIndex, string message);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void PrintHtmlCentre(int clientIndex, string message);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void PrintConsoleAll(string message);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void PrintChatAll(string message);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void PrintCentreAll(string message);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void PrintAlertAll(string message);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void PrintHtmlCentreAll(string message);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void PrintChatColored(int clientIndex, string message);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void PrintChatColoredAll(string message);
		
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr CreateConVarBool(string name, bool defaultValue, string description, int flags, bool hasMin, bool min, bool hasMax, bool max);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr CreateConVarInt16(string name, short defaultValue, string description, int flags, bool hasMin, short min, bool hasMax, short max);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr CreateConVarUInt16(string name, ushort defaultValue, string description, int flags, bool hasMin, ushort min, bool hasMax, ushort max);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr CreateConVarInt32(string name, int defaultValue, string description, int flags, bool hasMin, int min, bool hasMax, int max);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr CreateConVarUInt32(string name, uint defaultValue, string description, int flags, bool hasMin, uint min, bool hasMax, uint max);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr CreateConVarInt64(string name, long defaultValue, string description, int flags, bool hasMin, long min, bool hasMax, long max);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr CreateConVarUInt64(string name, ulong defaultValue, string description, int flags, bool hasMin, ulong min, bool hasMax, ulong max);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr CreateConVarFloat(string name, float defaultValue, string description, int flags, bool hasMin, float min, bool hasMax, float max);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr CreateConVarDouble(string name, double defaultValue, string description, int flags, bool hasMin, double min, bool hasMax, double max);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr CreateConVar(string name, string defaultValue, string description, int flags);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr CreateConVarColor(string name, int defaultValue, string description, int flags, bool hasMin, int min, bool hasMax, int max);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr CreateConVarVector2(string name, Vector2 defaultValue, string description, int flags, bool hasMin, Vector2 min, bool hasMax, Vector2 max);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr CreateConVarVector3(string name, Vector3 defaultValue, string description, int flags, bool hasMin, Vector3 min, bool hasMax, Vector3 max);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr CreateConVarVector4(string name, Vector4 defaultValue, string description, int flags, bool hasMin, Vector4 min, bool hasMax, Vector4 max);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr CreateConVarQangle(string name, Vector3 defaultValue, string description, int flags, bool hasMin, Vector3 min, bool hasMax, Vector3 max);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr FindConVar(string name);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void HookConVarChange(string name, ChangeCallback callback);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void UnhookConVarChange(string name, ChangeCallback callback);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern bool IsConVarFlagSet(IntPtr conVar, long flag);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void AddConVarFlags(IntPtr conVar, long flags);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void RemoveConVarFlags(IntPtr conVar, long flags);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern long GetConVarFlags(IntPtr conVar);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetConVarBounds(IntPtr conVar, bool max, string value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern string GetConVarValue(IntPtr conVar);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetConVarValue(IntPtr conVar, string value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SendConVarValue(IntPtr conVar, int clientIndex, string value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern string GetClientConVarValue(int clientIndex, string convarName);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetFakeClientConVarValue(int clientIndex, string convarName, string convarValue);
		
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern string GetGameDirectory();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern string GetMapName();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern bool IsMapValid(string mapname);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern float GetGameTime();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetGameTickCount();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern float GetGameFrameTime();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern double GetEngineTime();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetMaxClients();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int PrecacheModel(string model);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void IsModelPrecache(string model);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern bool PrecacheSound(string sound, bool preload);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern bool IsSoundPrecached(string sound);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int PrecacheDecal(string decal, bool preload);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern bool IsDecalPrecached(string decal);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern float GetTickInterval();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern double GetTickedTime();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr GetEconItemSystem();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern bool IsServerPaused();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void QueueTaskForNextFrame(TaskCallback callback);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void QueueTaskForNextWorldUpdate(TaskCallback callback);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern float GetSoundDuration(string name);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void EmitSound(int clientIndex, string sound, float volume);
		
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr GetEntityFromIndex(int entityIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetIndexFromEntity(IntPtr entity);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetUserIdFromIndex(int entityIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern string GetEntityClassname(IntPtr entity);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr GetEntityPointerFromHandle(IntPtr handle);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr GetEntityPointerFromRef(uint ref_);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern uint GetRefFromEntityPointer(IntPtr pEntity);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern bool IsRefValidEntity(uint ref_);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr GetFirstActiveEntity();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr GetConcreteEntityListPointer();

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void HookEntityOutput(string szClassname, string szOutput, HookEntityOutputCallback callback, bool post);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void UnhookEntityOutput(string szClassname, string szOutput, HookEntityOutputCallback callback, bool post);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int FindEntityByClassname(int startEntity, string szName);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int FindEntityByName(int startEntity, string szName);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int CreateEntityByName(string className);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void DispatchSpawn(int entityIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern string GetEntityName(int entityIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetEntityName(int entityIndex, string name);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetEntityMoveType(int entityIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetEntityMoveType(int entityIndex, int moveType);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern float GetEntityGravity(int entityIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetEntityGravity(int entityIndex, float gravity);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetEntityFlags(int entityIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetEntityFlags(int entityIndex, int flags);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetEntityRenderColor(int entityIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetEntityRenderColor(int entityIndex, int color);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern sbyte GetEntityRenderMode(int entityIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetEntityRenderMode(int entityIndex, sbyte renderMode);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetEntityHealth(int entityIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetEntityHealth(int entityIndex, int health);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetTeamEntity(int entityIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetTeamEntity(int entityIndex, int team);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetEntityOwner(int entityIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetEntityOwner(int entityIndex, int ownerIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void GetEntityAbsOrigin(ref Vector3 output, int entityIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetEntityAbsOrigin(int entityIndex, Vector3 origin);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void GetEntityAngRotation(ref Vector3 output, int entityIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetEntityAngRotation(int entityIndex, Vector3 angle);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void GetEntityAbsVelocity(ref Vector3 output, int entityIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetEntityAbsVelocity(int entityIndex, Vector3 velocity);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern float GetEntityWaterLevel(int entityIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetEntityGroundEntity(int entityIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetEntityEffects(int entityIndex);
		
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int HookEvent(string name, EventCallback pCallback, bool post);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int UnhookEvent(string name, EventCallback pCallback, bool post);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr CreateEvent(string name, bool force);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void FireEvent(IntPtr pInfo, bool bDontBroadcast);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void FireEventToClient(IntPtr pInfo, int entityIndex);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void CancelCreatedEvent(IntPtr pInfo);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern bool GetEventBool(IntPtr pInfo, string key);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern float GetEventFloat(IntPtr pInfo, string key);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetEventInt(IntPtr pInfo, string key);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern ulong GetEventUInt64(IntPtr pInfo, string key);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern string GetEventString(IntPtr pInfo, string key);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr GetEventPtr(IntPtr pInfo, string key);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr GetEventPlayerController(IntPtr pInfo, string key);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetEventPlayerIndex(IntPtr pInfo, string key);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr GetEventPlayerPawn(IntPtr pInfo, string key);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr GetEventEntity(IntPtr pInfo, string key);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetEventEntityIndex(IntPtr pInfo, string key);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern string GetEventName(IntPtr pInfo);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetEventBool(IntPtr pInfo, string key, bool value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetEventFloat(IntPtr pInfo, string key, float value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetEventInt(IntPtr pInfo, string key, int value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetEventUInt64(IntPtr pInfo, string key, ulong value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetEventString(IntPtr pInfo, string key, string value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetEventPtr(IntPtr pInfo, string key, IntPtr value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetEventPlayerController(IntPtr pInfo, string key, IntPtr value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetEventPlayerIndex(IntPtr pInfo, string key, int value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetEventEntity(IntPtr pInfo, string key, IntPtr value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetEventEntityIndex(IntPtr pInfo, string key, int value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetEventBroadcast(IntPtr pInfo, bool dontBroadcast);
		
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int LoadEventsFromFile(string path, bool searchAll);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void CloseGameConfigFile(IntPtr pGameConfig);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr LoadGameConfigFile(string file);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern string GetGameConfigPath(IntPtr pGameConfig);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern string GetGameConfigLibrary(IntPtr pGameConfig, string name);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern string GetGameConfigSignature(IntPtr pGameConfig, string name);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern string GetGameConfigSymbol(IntPtr pGameConfig, string name);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern string GetGameConfigPatch(IntPtr pGameConfig, string name);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetGameConfigOffset(IntPtr pGameConfig, string name);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr GetGameConfigAddress(IntPtr pGameConfig, string name);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr GetGameConfigMemSig(IntPtr pGameConfig, string name);
		
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int RegisterLoggingChannel(string name, int iFlags, int verbosity, int color);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void AddLoggerTagToChannel(int channelID, string tagName);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern bool HasLoggerTag(int channelID, string tag);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern bool IsLoggerChannelEnabledBySeverity(int channelID, int severity);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern bool IsLoggerChannelEnabledByVerbosity(int channelID, int verbosity);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetLoggerChannelVerbosity(int channelID);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetLoggerChannelVerbosity(int channelID, int verbosity);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetLoggerChannelVerbosityByName(int channelID, string name, int verbosity);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetLoggerChannelVerbosityByTag(int channelID, string tag, int verbosity);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetLoggerChannelColor(int channelID);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetLoggerChannelColor(int channelID, int color);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetLoggerChannelFlags(int channelID);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetLoggerChannelFlags(int channelID, int eFlags);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int Log(int channelID, int severity, string message);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int LogColored(int channelID, int severity, int color, string message);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int LogFull(int channelID, int severity, string file, int line, string function, string message);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int LogFullColored(int channelID, int severity, string file, int line, string function, int color, string message);
		
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetSchemaOffset(string className, string memberName);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern bool IsSchemaFieldNetworked(string className, string memberName);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetSchemaClassSize(string className);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern bool GetSchemaBoolByName(IntPtr instancePointer, string className, string memberName);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern sbyte GetSchemaInt8ByName(IntPtr instancePointer, string className, string memberName);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern short GetSchemaInt16ByName(IntPtr instancePointer, string className, string memberName);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern int GetSchemaInt32ByName(IntPtr instancePointer, string className, string memberName);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern long GetSchemaInt64ByName(IntPtr instancePointer, string className, string memberName);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern byte GetSchemaUInt8ByName(IntPtr instancePointer, string className, string memberName);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern ushort GetSchemaUInt16ByName(IntPtr instancePointer, string className, string memberName);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern uint GetSchemaUInt32ByName(IntPtr instancePointer, string className, string memberName);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern ulong GetSchemaUInt64ByName(IntPtr instancePointer, string className, string memberName);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern float GetSchemaFloatByName(IntPtr instancePointer, string className, string memberName);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern double GetSchemaDoubleByName(IntPtr instancePointer, string className, string memberName);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr GetSchemaPointerByName(IntPtr instancePointer, string className, string memberName);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern string GetSchemaStringByName(IntPtr instancePointer, string className, string memberName);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void GetSchemaVectorByName(ref Vector3 output, IntPtr instancePointer, string className, string memberName);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetSchemaValueBoolByName(IntPtr instancePointer, string className, string memberName, bool value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetSchemaValueInt8ByName(IntPtr instancePointer, string className, string memberName, sbyte value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetSchemaValueInt16ByName(IntPtr instancePointer, string className, string memberName, short value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetSchemaValueInt32ByName(IntPtr instancePointer, string className, string memberName, int value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetSchemaValueInt64ByName(IntPtr instancePointer, string className, string memberName, long value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetSchemaValueUInt8ByName(IntPtr instancePointer, string className, string memberName, byte value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetSchemaValueUInt16ByName(IntPtr instancePointer, string className, string memberName, ushort value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetSchemaValueUInt32ByName(IntPtr instancePointer, string className, string memberName, uint value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetSchemaValueUInt64ByName(IntPtr instancePointer, string className, string memberName, ulong value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetSchemaValueFloatByName(IntPtr instancePointer, string className, string memberName, float value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetSchemaValueDoubleByName(IntPtr instancePointer, string className, string memberName, double value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetSchemaValueStringByName(IntPtr instancePointer, string className, string memberName, string value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetSchemaValueVectorByName(IntPtr instancePointer, string className, string memberName, Vector3 value);
		
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern IntPtr CreateTimer(float interval, TimerCallback callback, int flags);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void KillsTimer(IntPtr timer);
		
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetClientListening(int receiver, int sender, sbyte listen);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern sbyte GetClientListening(int receiver, int sender);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern void SetClientVoiceFlags(int clientIndex, byte flags);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal static extern byte GetClientVoiceFlags(int clientIndex);
	}
	
	// Hand written wrappers

	internal class GameConfig : IDisposable
    {
        private readonly GameConfigHandle _gameConfig;

        public GameConfig(string file)
        {
            if (string.IsNullOrWhiteSpace(file))
            {
                throw new ArgumentException("File cannot be empty");
            }
            
            _gameConfig = new GameConfigHandle(cs2sdk.LoadGameConfigFile(file));
            
            if (_gameConfig.IsInvalid)
            {
                throw new InvalidOperationException($"Could not read \"${file}\" config.");
            }
        }

        public string GetPath()
        {
            return cs2sdk.GetGameConfigPath(_gameConfig.Handle);
        }

        public string GetLibrary(string name)
        {
            if (string.IsNullOrWhiteSpace(name))
            {
                throw new ArgumentException("Name cannot be empty");
            }
            
            return cs2sdk.GetGameConfigLibrary(_gameConfig.Handle, name);
        }

        public string GetSignature(string name)
        {
            if (string.IsNullOrWhiteSpace(name))
            {
                throw new ArgumentException("Name cannot be empty");
            }
            
            return cs2sdk.GetGameConfigSignature(_gameConfig.Handle, name);
        }

        public string GetSymbol(string name)
        {
            if (string.IsNullOrWhiteSpace(name))
            {
                throw new ArgumentException("Name cannot be empty");
            }
            
            return cs2sdk.GetGameConfigSymbol(_gameConfig.Handle, name);
        }

        public string GetPatch(string name)
        {
            if (string.IsNullOrWhiteSpace(name))
            {
                throw new ArgumentException("Name cannot be empty");
            }
            
            return cs2sdk.GetGameConfigPatch(_gameConfig.Handle, name);
        }

        public int GetOffset(string name)
        {
            if (string.IsNullOrWhiteSpace(name))
            {
                throw new ArgumentException("Name cannot be empty");
            }
            
            return cs2sdk.GetGameConfigOffset(_gameConfig.Handle, name);
        }
        
        public IntPtr GetAddress(string name)
        {
            if (string.IsNullOrWhiteSpace(name))
            {
                throw new ArgumentException("Name cannot be empty");
            }
            
            return cs2sdk.GetGameConfigAddress(_gameConfig.Handle, name);
        }
        
        public IntPtr GetMemSig(string name) 
        {
            if (string.IsNullOrWhiteSpace(name))
            {
                throw new ArgumentException("Name cannot be empty");
            }

            return cs2sdk.GetGameConfigMemSig(_gameConfig.Handle, name);
        }

        public void Dispose()
        {
            _gameConfig.Dispose();
        }
    }
    
    internal class GameConfigHandle : SafeHandleZeroOrMinusOneIsInvalid
    {
        public GameConfigHandle(IntPtr handle) : base(true)
        {
            this.handle = handle;
        }

        public IntPtr Handle => handle;
        
        protected override bool ReleaseHandle()
        {
            cs2sdk.CloseGameConfigFile(handle);
            return true;
        }
    }
	
	
	// TODO: Add more wrappers
}
