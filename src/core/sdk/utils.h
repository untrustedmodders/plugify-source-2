#pragma once

#include "schema.h"
#include <convar.h>
#include <eiface.h>
#if S2SDK_PLATFORM_LINUX || S2SDK_PLATFORM_APPLE
#include <cxxabi.h>
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


	inline void SetConVarStringByHandle(uint64 conVarHandle, const plg::string& value, bool replicate, bool notify) {
		ConVarRef conVarRef(conVarHandle);
	
		if (!conVarRef.IsValidRef()) {
			S2_LOG(LS_WARNING, "Invalid convar handle.\n");
			return;
		}

		auto* conVarData = g_pCVar->GetConVarData(conVarRef);
		if (conVarData == nullptr) {
			S2_LOG(LS_WARNING, "Invalid convar handle. Ensure the ConVarRef is correctly initialized and not null.\n");
			return;
		}

		ConVarRefAbstract conVar(conVarRef, conVarData);

		SetConVarString(conVar, value, replicate, notify);
	}

	template<typename T>
	void SetConVarInternal(ConVarRefAbstract conVar, const T& value, bool replicate, bool notify) {
		conVar.SetAs<T>(value);

		if (notify || replicate) {
			if constexpr (std::same_as<T, CUtlString>) {
				if (replicate) ReplicateConVar(conVar, value.Get());
				if (notify) NotifyConVar(conVar, value.Get());
			} else {
				plg::string val;
				if constexpr (std::is_same_v<T, Color>) {
					val = std::format("{} {} {} {}", value.r(), value.g(), value.b(), value.a());
				} else if constexpr (std::is_same_v<T, Vector2D>) {
					val = std::format("{} {}", value.x, value.y);
				} else if constexpr (std::is_same_v<T, Vector> || std::is_same_v<T, QAngle>) {
					val = std::format("{} {} {}", value.x, value.y, value.z);
				} else if constexpr (std::is_same_v<T, Vector4D>) {
					val = std::format("{} {} {} {}", value.x, value.y, value.z, value.w);
				} else {
					val = std::to_string(value);
				}
				if (replicate) ReplicateConVar(conVar, val.c_str());
				if (notify) NotifyConVar(conVar, val.c_str());
			}
		}
	}

	template<typename T>
	void SetConVar(ConVarRefAbstract conVar, const T& value, bool replicate, bool notify) {
		if (conVar.GetType() != TranslateConVarType<T>()) {
			S2_LOGF(LS_WARNING, "Invalid cvar type for variable '{}'. Expected: '{}', but got: '{}'. Please ensure the type matches the expected type.\n", conVar.GetName(), conVar.TypeTraits()->m_TypeName, GetCvarTypeTraits(TranslateConVarType<T>())->m_TypeName);
			return;
		}

		SetConVarInternal<T>(conVar, value, replicate, notify);
	}

	template<typename T>
	void SetConVarByHandle(uint64 conVarHandle, const T& value, bool replicate, bool notify) {
		ConVarRef conVarRef(conVarHandle);

		if (!conVarRef.IsValidRef()) {
			S2_LOG(LS_WARNING, "Invalid convar handle.\n");
			return;
		}

		auto* conVarData = g_pCVar->GetConVarData(conVarRef);
		if (conVarData == nullptr) {
			S2_LOG(LS_WARNING, "Invalid convar handle. Ensure the ConVarRef is correctly initialized and not null.\n");
			return;
		}

		ConVarRefAbstract conVar(conVarRef, conVarData);

		SetConVar<T>(conVar, value, replicate, notify);
	}

	template<typename T>
	void SetConVarValue(ConVarRefAbstract conVar, const plg::string& value) {
		if (conVar.GetType() != TranslateConVarType<T>()) {
			S2_LOGF(LS_WARNING, "Invalid cvar type for variable '{}'. Expected: '{}', but got: '{}'. Please ensure the type matches the expected type.\n", conVar.GetName(), conVar.TypeTraits()->m_TypeName, GetCvarTypeTraits(TranslateConVarType<T>())->m_TypeName);
			return;
		}

		conVar.SetString(value.c_str());
	}

	template<typename T>
	T GetConVarValue(ConVarRefAbstract conVar) {
		if (conVar.GetType() != TranslateConVarType<T>()) {
			S2_LOGF(LS_WARNING, "Invalid cvar type for variable '{}'. Expected: '{}', but got: '{}'. Please ensure the type matches the expected type.\n", conVar.GetName(), conVar.TypeTraits()->m_TypeName, GetCvarTypeTraits(TranslateConVarType<T>())->m_TypeName);
			return {};
		}

		return conVar.GetAs<T>();
	}

	template<typename T>
	T GetConVarValueByHandle(uint64 conVarHandle) {
		ConVarRef conVarRef(conVarHandle);
	
		if (!conVarRef.IsValidRef()) {
			S2_LOG(LS_WARNING, "Invalid convar handle.\n");
			return {};
		}

		auto* conVarData = g_pCVar->GetConVarData(conVarRef);
		if (conVarData == nullptr) {
			S2_LOG(LS_WARNING, "Invalid convar handle. Ensure the ConVarRef is correctly initialized and not null.\n");
			return {};
		}

		ConVarRefAbstract conVar(conVarRef, conVarData);

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

	std::vector<std::string_view> split(std::string_view strv, std::string_view delims);

	template<typename T>
	std::optional<T> string_to_int(std::string_view str, int base = 10) {
		T value;
		auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value, base);
		if (ec == std::errc()) {
			return value;
		}
		return std::nullopt;
	}

	template<typename T>
	std::vector<T> string_to_vector(std::string_view str) {
		std::vector<T> vec;
		for (const auto& item : split(str, ",")) {
			if (auto value = string_to_int<T>(item)) {
				vec.emplace_back(*value);
			}
		}
		return vec;
	}

	template<typename T>
	plg::string vector_to_string(const std::vector<T>& vec) {
		if (vec.empty()) {
			return {};
		}
		plg::string buffer(plg::to_string(vec[0]));
		for (auto it = std::next(vec.begin()); it != vec.end(); ++it) {
			std::format_to(std::back_inserter(buffer), ",{}", *it);
		}
		return buffer;
	}

	namespace {
		template<typename T, typename... Rest>
		void hash_combine(std::size_t& seed, const T& v, const Rest&... rest) {
			seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			(hash_combine(seed, rest), ...);
		}
	}// namespace

	template<typename T1, typename T2>
	struct pair_hash {
		std::size_t operator()(std::pair<T1, T2> const& p) const {
			std::size_t seed{};
			hash_combine(seed, p.first, p.second);
			return seed;
		}
	};

	struct case_ins_comparator {
		bool operator()(const plg::string& lhs, const plg::string& rhs) const {
			return std::lexicographical_compare(
					lhs.begin(), lhs.end(),
					rhs.begin(), rhs.end(),
					[](char a, char b) { return std::tolower(a) < std::tolower(b); });
		}
	};

	struct string_hash {
		using is_transparent = void;
		[[nodiscard]] size_t operator()(const char* txt) const {
			return std::hash<std::string_view>{}(txt);
		}

		[[nodiscard]] size_t operator()(std::string_view txt) const {
			return std::hash<std::string_view>{}(txt);
		}

		[[nodiscard]] size_t operator()(const std::string& txt) const {
			return std::hash<std::string>{}(txt);
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
