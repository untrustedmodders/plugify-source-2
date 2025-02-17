#pragma once

#include "schema.h"
#include <convar.h>
#include <eiface.h>
#if S2SDK_PLATFORM_LINUX || S2SDK_PLATFORM_APPLE
#include <cxxabi.h>
#endif

#if S2SDK_PLATFORM_WINDOWS
#define S2SDK_NSTR(str) L##str
#define S2SDK_UTF8(str) utils::ConvertWideToUtf8(str)
#else
#define S2SDK_NSTR(str) str
#define S2SDK_UTF8(str) str
#endif


class CBaseEntity;
class CServerSideClientBase;

#include <variant>

#include <icvar.h>
#include <tier1/convar.h>

namespace utils {
	// ConVars
	void SendConVarValue(CPlayerSlot slot, const char* name, const char* value);
	void SendMultipleConVarValues(CPlayerSlot slot, const char** names, const char** values, uint32_t size);

	CBaseEntity* FindEntityByClassname(CEntityInstance* start, const char* name);

	CBasePlayerController* GetController(CBaseEntity* entity);
	CBasePlayerController* GetController(CPlayerSlot slot);
	bool IsPlayerSlot(CPlayerSlot slot);

	CPlayerSlot GetEntityPlayerSlot(CBaseEntity* entity);
	CUtlVector<CServerSideClientBase*>* GetClientList();
	CServerSideClientBase* GetClientBySlot(CPlayerSlot slot);

	// Normalize the angle between -180 and 180.
	float NormalizeDeg(float a);
	// Gets the difference in angle between 2 angles.
	// c can be PI (for radians) or 180.0 (for degrees);
	float GetAngleDifference(float x, float y, float c, bool relative = false);

	void NotifyConVar(const ConVarRefAbstract& conVar, const char* value);
	void ReplicateConVar(const ConVarRefAbstract& conVar, const char* value);

	inline void SetConVarString(ConVarRefAbstract conVar, const plg::string& value, bool replicate, bool notify) {
		conVar.SetString(value.c_str(), -1);
		if (replicate) ReplicateConVar(conVar, value.c_str());
		if (notify) NotifyConVar(conVar, value.c_str());
	}


	inline void SetConVarStringByHandle(ConVarRef conVarHandle, const plg::string& value, bool replicate, bool notify) {
		auto* conVarData = g_pCVar->GetConVarData(conVarHandle);
		if (conVarData == nullptr) {
			g_Logger.Log(LS_WARNING, "Invalid convar handle. Ensure the ConVarRef is correctly initialized and not null.\n");
			return;
		}

		ConVarRefAbstract conVar(conVarHandle, conVarData);

		SetConVarString(conVar, value, replicate, notify);
	}

	template<typename T>
	void SetConVarInternal(ConVarRefAbstract conVar, T value, bool replicate, bool notify) {
		conVar.SetAs<T>(value);
		if constexpr (std::same_as<T, const char*>) {
			if (replicate) ReplicateConVar(conVar, value);
			if (notify) NotifyConVar(conVar, value);
		} else {
			if (replicate || notify) {
				CUtlString value = conVar.GetString();
				if (replicate) ReplicateConVar(conVar, value.Get());
				if (notify) NotifyConVar(conVar, value.Get());
			}
		}
	}

	template<typename T>
	void SetConVar(ConVarRefAbstract conVar, T value, bool replicate, bool notify) {
		if (conVar.GetType() != TranslateConVarType<T>()) {
			g_Logger.LogFormat(LS_WARNING, "Invalid cvar type for variable '%s'. Expected: '%s', but got: '%s'. Please ensure the type matches the expected type.\n", conVar.GetName(), conVar.TypeTraits()->m_TypeName, GetCvarTypeTraits(TranslateConVarType<T>())->m_TypeName);
			return;
		}

		SetConVarInternal<T>(conVar, value, replicate, notify);
	}

	template<typename T>
	void SetConVarByHandle(ConVarRef conVarHandle, T value, bool replicate, bool notify) {
		auto* conVarData = g_pCVar->GetConVarData(conVarHandle);
		if (conVarData == nullptr) {
			g_Logger.Log(LS_WARNING, "Invalid convar handle. Ensure the ConVarRef is correctly initialized and not null.\n");
			return;
		}

		ConVarRefAbstract conVar(conVarHandle, conVarData);

		SetConVar<T>(conVar, value, replicate, notify);
	}

	template<typename T>
	void SetConVarValue(ConVarRefAbstract conVar, const plg::string& value) {
		if (conVar.GetType() != TranslateConVarType<T>()) {
			g_Logger.LogFormat(LS_WARNING, "Invalid cvar type for variable '%s'. Expected: '%s', but got: '%s'. Please ensure the type matches the expected type.\n", conVar.GetName(), conVar.TypeTraits()->m_TypeName, GetCvarTypeTraits(TranslateConVarType<T>())->m_TypeName);
			return;
		}

		conVar.SetString(value.c_str());
	}

	template<typename T>
	T GetConVarValue(ConVarRefAbstract conVar) {
		if (conVar.GetType() != TranslateConVarType<T>()) {
			g_Logger.LogFormat(LS_WARNING, "Invalid cvar type for variable '%s'. Expected: '%s', but got: '%s'. Please ensure the type matches the expected type.\n", conVar.GetName(), conVar.TypeTraits()->m_TypeName, GetCvarTypeTraits(TranslateConVarType<T>())->m_TypeName);
			return {};
		}

		return conVar.GetAs<T>();
	}

	template<typename T>
	T GetConVarValueByHandle(ConVarRef conVarHandle) {
		auto* conVarData = g_pCVar->GetConVarData(conVarHandle);
		if (conVarData == nullptr) {
			g_Logger.Log(LS_WARNING, "Invalid convar handle. Ensure the ConVarRef is correctly initialized and not null.\n");
			return {};
		}

		ConVarRefAbstract conVar(conVarHandle, conVarData);

		return GetConVarValue<T>(conVar);
	}

	// Print functions
	bool CFormat(char* buffer, uint64_t buffer_size, const char* text);
	void ClientPrintFilter(IRecipientFilter* filter, int msg_dest, const char* msg_name);
	void PrintConsole(CPlayerSlot slot, const char* message);
	void PrintChat(CPlayerSlot slot, const char* message);
	void PrintCentre(CPlayerSlot slot, const char* message);
	void PrintAlert(CPlayerSlot slot, const char* message);
	void PrintHtmlCentre(CPlayerSlot slot, const char* message);// This one uses HTML formatting.
	void PrintConsoleAll(const char* message);
	void PrintChatAll(const char* message);
	void PrintCentreAll(const char* message);
	void PrintAlertAll(const char* message);
	void PrintHtmlCentreAll(const char* message);// This one uses HTML formatting.

	// Color print
	void CPrintChat(CPlayerSlot slot, const char* message);
	void CPrintChatAll(const char* message);

	// Sounds
	void PlaySoundToClient(CPlayerSlot player, int channel, const char* soundName, float volume, soundlevel_t soundLevel, int flags, int pitch, const Vector& origin, float soundTime);

	// Return true if the spawn found is truly valid (not in the ground or out of bounds)
	bool IsSpawnValid(const Vector& origin);
	bool FindValidSpawn(Vector& origin, QAngle& angles);

	const plg::string& GameDirectory();

	std::vector<plg::string> Split(std::string_view strv, std::string_view delims);

	bool ParseInt(std::string_view str, int& out, int base = 10);

#if S2SDK_PLATFORM_WINDOWS
	/// Converts the specified UTF-8 string to a wide string.
	plg::wstring ConvertUtf8ToWide(std::string_view str);
	bool ConvertUtf8ToWide(plg::wstring& dest, std::string_view str);

	/// Converts the specified wide string to a UTF-8 string.
	plg::string ConvertWideToUtf8(std::wstring_view str);
	bool ConvertWideToUtf8(plg::string& dest, std::wstring_view str);
#endif

	namespace {
		template<typename T, typename... Rest>
		void hash_combine(std::size_t& seed, const T& v, const Rest&... rest) {
			seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			(hash_combine(seed, rest), ...);
		}
	}// namespace

	template<typename T1, typename T2>
	struct PairHash {
		std::size_t operator()(std::pair<T1, T2> const& p) const {
			std::size_t seed{};
			hash_combine(seed, p.first, p.second);
			return seed;
		}
	};

	struct CaseInsensitiveComparator {
		bool operator()(const plg::string& lhs, const plg::string& rhs) const {
			return std::lexicographical_compare(
					lhs.begin(), lhs.end(),
					rhs.begin(), rhs.end(),
					[](char a, char b) { return std::tolower(a) < std::tolower(b); });
		}
	};

}// namespace utils

enum class FieldType : int {
	Auto,
	Float32,
	Float64,
	Int32,
	UInt32,
	Int64,
	UInt64,
	Boolean,
	Character,
	String,
	CString,
	HScript,
	EHandle,
	Resource,
	Vector3d,
	Vector2d,
	Vector4d,
	Color32,
	QAngle,
	Quaternion
};
