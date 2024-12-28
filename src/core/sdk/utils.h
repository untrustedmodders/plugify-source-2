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

namespace utils {
	// ConVars
	void SendConVarValue(CPlayerSlot slot, BaseConVar* cvar, const char* value);
	void SendMultipleConVarValues(CPlayerSlot slot, BaseConVar** cvars, const char** values, uint32_t size);

	CBaseEntity* FindEntityByClassname(CEntityInstance* start, const char* name);

	CBasePlayerController* GetController(CBaseEntity* entity);
	CBasePlayerController* GetController(CPlayerSlot slot);
	bool IsPlayerSlot(CPlayerSlot slot);

	CPlayerSlot GetSlotFromUserId(uint16 userid);
	CPlayerSlot GetEntityPlayerSlot(CBaseEntity* entity);
	CUtlVector<CServerSideClientBase*>* GetClientList();
	CServerSideClientBase* GetClientBySlot(CPlayerSlot slot);

	// Normalize the angle between -180 and 180.
	float NormalizeDeg(float a);
	// Gets the difference in angle between 2 angles.
	// c can be PI (for radians) or 180.0 (for degrees);
	float GetAngleDifference(float x, float y, float c, bool relative = false);

	void NotifyConVar(BaseConVar* conVar, const char* value);
	void ReplicateConVar(BaseConVar* conVar, const char* value);

	template<typename T>
	void SetConVar(BaseConVar* conVar, const T& value, bool replicate, bool notify) {
		if (conVar == nullptr) {
			g_Logger.Log(LS_WARNING, "Invalid cvar pointer\n");
			return;
		}

		auto* conVarData = conVar->GetConVarData();
		if (conVarData == nullptr) {
			g_Logger.Log(LS_WARNING, "Invalid cvar data\n");
			return;
		}

		if (conVarData->GetType() != TranslateConVarType<T>()) {
			g_Logger.Log(LS_WARNING, "Invalid cvar type\n");
			return;
		}

		static_cast<ConVar<T>*>(conVar)->SetValue(value);
		if constexpr (std::same_as<T, const char*>) {
			if (replicate) ReplicateConVar(conVar, value);
			if (notify) NotifyConVar(conVar, value);
		} else {
			if (replicate || notify) {
				char val[512];
				conVarData->Cast<T>()->GetStringValue(val, sizeof(val));
				if (replicate) ReplicateConVar(conVar, val);
				if (notify) NotifyConVar(conVar, val);
			}
		}
	}

	template<typename T>
	void SetConVarString(BaseConVar* conVar, const plg::string& value, bool replicate, bool notify) {
		if (conVar == nullptr) {
			g_Logger.Log(LS_WARNING, "Invalid cvar pointer\n");
			return;
		}

		auto* conVarData = conVar->GetConVarData();
		if (conVarData == nullptr) {
			g_Logger.Log(LS_WARNING, "Invalid cvar data\n");
			return;
		}

		if (conVarData->GetType() != TranslateConVarType<T>()) {
			g_Logger.Log(LS_WARNING, "Invalid cvar type\n");
			return;
		}

		static_cast<ConVar<T>*>(conVar)->SetStringValue(value.c_str(), value.size());
		if (replicate) ReplicateConVar(conVar, value.c_str());
		if (notify) NotifyConVar(conVar, value.c_str());
	}

	template<typename T>
	void SetConVarValue(BaseConVar* conVar, const T& value) {
		if (conVar == nullptr) {
			g_Logger.Log(LS_WARNING, "Invalid convar pointer.\n");
			return;
		}

		auto* conVarData = conVar->GetConVarData();
		if (conVarData == nullptr) {
			g_Logger.Log(LS_WARNING, "Invalid convar data.\n");
			return;
		}

		static_cast<ConVar<T>*>(conVar)->SetValue(value);
	}

	template<typename T>
	T GetConVarValue(BaseConVar* conVar) {
		if (conVar == nullptr) {
			g_Logger.Log(LS_WARNING, "Invalid convar pointer.\n");
			return {};
		}

		auto* conVarData = conVar->GetConVarData();
		if (conVarData == nullptr) {
			g_Logger.Log(LS_WARNING, "Invalid convar data.\n");
			return {};
		}

		return conVarData->Cast<T>()->GetValue();
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
