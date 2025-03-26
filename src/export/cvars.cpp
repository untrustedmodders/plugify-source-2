#include <convar.h>
#include <core/con_var_manager.hpp>
#include <core/game_config.hpp>
#include <core/sdk/utils.h>
#include <core/sdk/virtual.h>
#include <plugify/cpp_plugin.hpp>
#include <plugin_export.h>

#include <basetypes.h>

PLUGIFY_WARN_PUSH()

#if defined(__clang)
PLUGIFY_WARN_IGNORE("-Wreturn-type-c-linkage")
#elif defined(_MSC_VER)
PLUGIFY_WARN_IGNORE(4190)
#endif

/**
 * @brief Creates a new console variable.
 * @param name The name of the console variable.
 * @param defaultValue The default value of the console variable.
 * @param description A description of the console variable's purpose.
 * @param flags Additional flags for the console variable.
 * @return A handle to the created console variable.
 */
extern "C" PLUGIN_API uint64_t CreateConVar(const plg::string& name, const plg::string& defaultValue, const plg::string& description, ConVarFlag flags) {
	return g_ConVarManager.CreateConVar<CUtlString>(name.c_str(), description, defaultValue.data(), flags);
}

/**
 * @brief Creates a new boolean console variable.
 *
 * This function registers a new console variable of type boolean with the specified name, default value,
 * description, and flags. Additionally, it allows for setting minimum and maximum constraints.
 *
 * @param name The name of the console variable.
 * @param defaultValue The default value for the console variable.
 * @param description A brief description of the console variable.
 * @param flags Flags that define the behavior of the console variable.
 * @param hasMin Indicates if a minimum value is provided.
 * @param min The minimum value if hasMin is true.
 * @param hasMax Indicates if a maximum value is provided.
 * @param max The maximum value if hasMax is true.
 * @return A handle to the created console variable data.
 */
extern "C" PLUGIN_API uint64_t CreateConVarBool(const plg::string& name, bool defaultValue, const plg::string& description, ConVarFlag flags, bool hasMin, bool min, bool hasMax, bool max) {
	return g_ConVarManager.CreateConVar<bool>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

/**
 * @brief Creates a new 16-bit signed integer console variable.
 *
 * This function registers a new console variable of type int16 with the specified parameters.
 *
 * @param name The name of the console variable.
 * @param defaultValue The default value for the console variable.
 * @param description A brief description of the console variable.
 * @param flags Flags that define the behavior of the console variable.
 * @param hasMin Indicates if a minimum value is provided.
 * @param min The minimum value if hasMin is true.
 * @param hasMax Indicates if a maximum value is provided.
 * @param max The maximum value if hasMax is true.
 * @return A handle to the created console variable data.
 */
extern "C" PLUGIN_API uint64_t CreateConVarInt16(const plg::string& name, int16 defaultValue, const plg::string& description, ConVarFlag flags, bool hasMin, int16 min, bool hasMax, int16 max) {
	return g_ConVarManager.CreateConVar<int16>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

/**
 * @brief Creates a new 16-bit unsigned integer console variable.
 *
 * This function registers a new console variable of type uint16 with the specified parameters.
 *
 * @param name The name of the console variable.
 * @param defaultValue The default value for the console variable.
 * @param description A brief description of the console variable.
 * @param flags Flags that define the behavior of the console variable.
 * @param hasMin Indicates if a minimum value is provided.
 * @param min The minimum value if hasMin is true.
 * @param hasMax Indicates if a maximum value is provided.
 * @param max The maximum value if hasMax is true.
 * @return A handle to the created console variable data.
 */
extern "C" PLUGIN_API uint64_t CreateConVarUInt16(const plg::string& name, uint16 defaultValue, const plg::string& description, ConVarFlag flags, bool hasMin, uint16 min, bool hasMax, uint16 max) {
	return g_ConVarManager.CreateConVar<uint16>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

/**
 * @brief Creates a new 32-bit signed integer console variable.
 *
 * This function registers a new console variable of type int32 with the specified parameters.
 *
 * @param name The name of the console variable.
 * @param defaultValue The default value for the console variable.
 * @param description A brief description of the console variable.
 * @param flags Flags that define the behavior of the console variable.
 * @param hasMin Indicates if a minimum value is provided.
 * @param min The minimum value if hasMin is true.
 * @param hasMax Indicates if a maximum value is provided.
 * @param max The maximum value if hasMax is true.
 * @return A handle to the created console variable data.
 */
extern "C" PLUGIN_API uint64_t CreateConVarInt32(const plg::string& name, int32 defaultValue, const plg::string& description, ConVarFlag flags, bool hasMin, int32 min, bool hasMax, int32 max) {
	return g_ConVarManager.CreateConVar<int32>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

/**
 * @brief Creates a new 32-bit unsigned integer console variable.
 *
 * This function registers a new console variable of type uint32 with the specified parameters.
 *
 * @param name The name of the console variable.
 * @param defaultValue The default value for the console variable.
 * @param description A brief description of the console variable.
 * @param flags Flags that define the behavior of the console variable.
 * @param hasMin Indicates if a minimum value is provided.
 * @param min The minimum value if hasMin is true.
 * @param hasMax Indicates if a maximum value is provided.
 * @param max The maximum value if hasMax is true.
 * @return A handle to the created console variable data.
 */
extern "C" PLUGIN_API uint64_t CreateConVarUInt32(const plg::string& name, uint32 defaultValue, const plg::string& description, ConVarFlag flags, bool hasMin, uint32 min, bool hasMax, uint32 max) {
	return g_ConVarManager.CreateConVar<uint32>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}


/**
 * @brief Creates a new 64-bit signed integer console variable.
 *
 * This function registers a new console variable of type int64 with the specified parameters.
 *
 * @param name The name of the console variable.
 * @param defaultValue The default value for the console variable.
 * @param description A brief description of the console variable.
 * @param flags Flags that define the behavior of the console variable.
 * @param hasMin Indicates if a minimum value is provided.
 * @param min The minimum value if hasMin is true.
 * @param hasMax Indicates if a maximum value is provided.
 * @param max The maximum value if hasMax is true.
 * @return A handle to the created console variable data.
 */
extern "C" PLUGIN_API uint64_t CreateConVarInt64(const plg::string& name, int64 defaultValue, const plg::string& description, ConVarFlag flags, bool hasMin, int64 min, bool hasMax, int64 max) {
	return g_ConVarManager.CreateConVar<int64>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

/**
 * @brief Creates a new 64-bit unsigned integer console variable.
 *
 * This function registers a new console variable of type uint64 with the specified parameters.
 *
 * @param name The name of the console variable.
 * @param defaultValue The default value for the console variable.
 * @param description A brief description of the console variable.
 * @param flags Flags that define the behavior of the console variable.
 * @param hasMin Indicates if a minimum value is provided.
 * @param min The minimum value if hasMin is true.
 * @param hasMax Indicates if a maximum value is provided.
 * @param max The maximum value if hasMax is true.
 * @return A handle to the created console variable data.
 */
extern "C" PLUGIN_API uint64_t CreateConVarUInt64(const plg::string& name, uint64 defaultValue, const plg::string& description, ConVarFlag flags, bool hasMin, uint64 min, bool hasMax, uint64 max) {
	return g_ConVarManager.CreateConVar<uint64>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

/**
 * @brief Creates a new floating-point console variable.
 *
 * This function registers a new console variable of type float with the specified parameters.
 *
 * @param name The name of the console variable.
 * @param defaultValue The default value for the console variable.
 * @param description A brief description of the console variable.
 * @param flags Flags that define the behavior of the console variable.
 * @param hasMin Indicates if a minimum value is provided.
 * @param min The minimum value if hasMin is true.
 * @param hasMax Indicates if a maximum value is provided.
 * @param max The maximum value if hasMax is true.
 * @return A handle to the created console variable data.
 */
extern "C" PLUGIN_API uint64_t CreateConVarFloat(const plg::string& name, float defaultValue, const plg::string& description, ConVarFlag flags, bool hasMin, float min, bool hasMax, float max) {
	return g_ConVarManager.CreateConVar<float>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

/**
 * @brief Creates a new double-precision console variable.
 *
 * This function registers a new console variable of type double with the specified parameters.
 *
 * @param name The name of the console variable.
 * @param defaultValue The default value for the console variable.
 * @param description A brief description of the console variable.
 * @param flags Flags that define the behavior of the console variable.
 * @param hasMin Indicates if a minimum value is provided.
 * @param min The minimum value if hasMin is true.
 * @param hasMax Indicates if a maximum value is provided.
 * @param max The maximum value if hasMax is true.
 * @return A handle to the created console variable data.
 */
extern "C" PLUGIN_API uint64_t CreateConVarDouble(const plg::string& name, double defaultValue, const plg::string& description, ConVarFlag flags, bool hasMin, double min, bool hasMax, double max) {
	return g_ConVarManager.CreateConVar<double>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

/**
 * @brief Creates a new color console variable.
 *
 * This function registers a new console variable of type Color with the specified parameters.
 * The color is specified as an integer value.
 *
 * @param name The name of the console variable.
 * @param defaultValue The default color value for the console variable.
 * @param description A brief description of the console variable.
 * @param flags Flags that define the behavior of the console variable.
 * @param hasMin Indicates if a minimum value is provided.
 * @param min The minimum color value if hasMin is true.
 * @param hasMax Indicates if a maximum value is provided.
 * @param max The maximum color value if hasMax is true.
 * @return A handle to the created console variable data.
 */
extern "C" PLUGIN_API uint64_t CreateConVarColor(const plg::string& name, int defaultValue, const plg::string& description, ConVarFlag flags, bool hasMin, int min, bool hasMax, int max) {
	return g_ConVarManager.CreateConVar<Color>(name, description, *reinterpret_cast<Color*>(&defaultValue), flags, hasMin, *reinterpret_cast<Color*>(&min), hasMax, *reinterpret_cast<Color*>(&max));
}

/**
 * @brief Creates a new 2D vector console variable.
 *
 * This function registers a new console variable of type Vector2D with the specified parameters.
 *
 * @param name The name of the console variable.
 * @param defaultValue The default value for the console variable.
 * @param description A brief description of the console variable.
 * @param flags Flags that define the behavior of the console variable.
 * @param hasMin Indicates if a minimum value is provided.
 * @param min The minimum value if hasMin is true.
 * @param hasMax Indicates if a maximum value is provided.
 * @param max The maximum value if hasMax is true.
 * @return A handle to the created console variable data.
 */
extern "C" PLUGIN_API uint64_t CreateConVarVector2(const plg::string& name, const Vector2D& defaultValue, const plg::string& description, ConVarFlag flags, bool hasMin, const Vector2D& min, bool hasMax, const Vector2D& max) {
	return g_ConVarManager.CreateConVar<Vector2D>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

/**
 * @brief Creates a new 3D vector console variable.
 *
 * This function registers a new console variable of type Vector with the specified parameters.
 *
 * @param name The name of the console variable.
 * @param defaultValue The default value for the console variable.
 * @param description A brief description of the console variable.
 * @param flags Flags that define the behavior of the console variable.
 * @param hasMin Indicates if a minimum value is provided.
 * @param min The minimum value if hasMin is true.
 * @param hasMax Indicates if a maximum value is provided.
 * @param max The maximum value if hasMax is true.
 * @return A handle to the created console variable data.
 */
extern "C" PLUGIN_API uint64_t CreateConVarVector3(const plg::string& name, const Vector& defaultValue, const plg::string& description, ConVarFlag flags, bool hasMin, const Vector& min, bool hasMax, const Vector& max) {
	return g_ConVarManager.CreateConVar<Vector>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

/**
 * @brief Creates a new 4D vector console variable.
 *
 * This function registers a new console variable of type Vector4D with the specified parameters.
 *
 * @param name The name of the console variable.
 * @param defaultValue The default value for the console variable.
 * @param description A brief description of the console variable.
 * @param flags Flags that define the behavior of the console variable.
 * @param hasMin Indicates if a minimum value is provided.
 * @param min The minimum value if hasMin is true.
 * @param hasMax Indicates if a maximum value is provided.
 * @param max The maximum value if hasMax is true.
 * @return A handle to the created console variable data.
 */
extern "C" PLUGIN_API uint64_t CreateConVarVector4(const plg::string& name, const Vector4D& defaultValue, const plg::string& description, ConVarFlag flags, bool hasMin, const Vector4D& min, bool hasMax, const Vector4D& max) {
	return g_ConVarManager.CreateConVar<Vector4D>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

/**
 * @brief Creates a new quaternion angle console variable.
 *
 * This function registers a new console variable of type QAngle with the specified parameters.
 *
 * @param name The name of the console variable.
 * @param defaultValue The default value for the console variable.
 * @param description A brief description of the console variable.
 * @param flags Flags that define the behavior of the console variable.
 * @param hasMin Indicates if a minimum value is provided.
 * @param min The minimum value if hasMin is true.
 * @param hasMax Indicates if a maximum value is provided.
 * @param max The maximum value if hasMax is true.
 * @return A handle to the created console variable data.
 */
extern "C" PLUGIN_API uint64_t CreateConVarQAngle(const plg::string& name, const QAngle& defaultValue, const plg::string& description, ConVarFlag flags, bool hasMin, const QAngle& min, bool hasMax, const QAngle& max) {
	return g_ConVarManager.CreateConVar<QAngle>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

/**
 * @brief Searches for a console variable. *
 * @param name The name of the console variable to search for.
 * @return Pointer to the console variable data if found; otherwise, nullptr.
 */
extern "C" PLUGIN_API uint64_t FindConVar(const plg::string& name) {
	return g_ConVarManager.FindConVar(name);
}

/**
 * @brief Searches for a console variable of a specific type.
 * @param name The name of the console variable to search for.
 * @param type The type of the console variable to search for.
 * @return Pointer to the console variable data if found; otherwise, nullptr.
 */
extern "C" PLUGIN_API uint64_t FindConVar2(const plg::string& name, EConVarType type) {
	switch (type) {
		case EConVarType_Bool:
			return g_ConVarManager.FindConVar<bool>(name);
		case EConVarType_Int16:
			return g_ConVarManager.FindConVar<int16>(name);
		case EConVarType_UInt16:
			return g_ConVarManager.FindConVar<uint16>(name);
		case EConVarType_Int32:
			return g_ConVarManager.FindConVar<int32>(name);
		case EConVarType_UInt32:
			return g_ConVarManager.FindConVar<uint32>(name);
		case EConVarType_Int64:
			return g_ConVarManager.FindConVar<int64>(name);
		case EConVarType_UInt64:
			return g_ConVarManager.FindConVar<uint64>(name);
		case EConVarType_Float32:
			return g_ConVarManager.FindConVar<float>(name);
		case EConVarType_Float64:
			return g_ConVarManager.FindConVar<double>(name);
		case EConVarType_String:
			return g_ConVarManager.FindConVar<CUtlString>(name);
		case EConVarType_Color:
			return g_ConVarManager.FindConVar<Color>(name);
		case EConVarType_Vector2:
			return g_ConVarManager.FindConVar<Vector2D>(name);
		case EConVarType_Vector3:
			return g_ConVarManager.FindConVar<Vector>(name);
		case EConVarType_Vector4:
			return g_ConVarManager.FindConVar<Vector4D>(name);
		case EConVarType_Qangle:
			return g_ConVarManager.FindConVar<QAngle>(name);
		default:
			S2_LOG(LS_WARNING, "Invalid convar type.\n");
			return {};
	}
}

/**
 * @brief Creates a hook for when a console variable's value is changed.
*
 * This function allows a callback to be executed whenever the specified console variable is modified.
 *
 * @param name The name of the console variable to hook.
 * @param callback The callback function to be executed when the variable's value changes.
 */
extern "C" PLUGIN_API void HookConVarChange(const plg::string& name, ConVarChangeListenerCallback callback) {
	if (callback == nullptr) {
		S2_LOG(LS_WARNING, "Invalid callback pointer\n");
		return;
	}

	g_ConVarManager.HookConVarChange(name, callback);
}

/**
 * @brief Removes a hook for when a console variable's value is changed.
*
 * This function unhooks a previously set callback for a console variable.
 *
 * @param name The name of the console variable to unhook.
 * @param callback The callback function to be removed.
 */
extern "C" PLUGIN_API void UnhookConVarChange(const plg::string& name, ConVarChangeListenerCallback callback) {
	if (callback == nullptr) {
		S2_LOG(LS_WARNING, "Invalid callback pointer\n");
		return;
	}

	g_ConVarManager.UnhookConVarChange(name, callback);
}

/**
 * @brief Checks if a specific flag is set for a console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @param flag The flag to check against the console variable.
 * @return True if the flag is set; otherwise, false.
 */
extern "C" PLUGIN_API bool IsConVarFlagSet(uint64_t conVarHandle, ConVarFlag flag) {
	ConVarRef conVarRef(conVarHandle);

	if (!conVarRef.IsValidRef()) {
		S2_LOGF(LS_WARNING, "Invalid convar handle\n");
		return false;
	}

	auto* conVarData = g_pCVar->GetConVarData(conVarRef);
	if (conVarData == nullptr) {
		S2_LOG(LS_WARNING, "Invalid convar handle. Ensure the ConVarRef is correctly initialized and not null.\n");
		return false;
	}

	return conVarData->IsFlagSet(static_cast<int64>(flag));
}

/**
 * @brief Adds flags to a console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @param flags The flags to be added.
 */
extern "C" PLUGIN_API void AddConVarFlags(uint64_t conVarHandle, ConVarFlag flags) {
	ConVarRef conVarRef(conVarHandle);

	if (!conVarRef.IsValidRef()) {
		S2_LOGF(LS_WARNING, "Invalid convar handle\n");
		return;
	}

	auto* conVarData = g_pCVar->GetConVarData(conVarRef);
	if (conVarData == nullptr) {
		S2_LOG(LS_WARNING, "Invalid convar handle. Ensure the ConVarRef is correctly initialized and not null.\n");
		return;
	}

	conVarData->AddFlags(static_cast<int64>(flags));
}

/**
 * @brief Removes flags from a console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @param flags The flags to be removed.
 */
extern "C" PLUGIN_API void RemoveConVarFlags(uint64_t conVarHandle, ConVarFlag flags) {
	ConVarRef conVarRef(conVarHandle);

	if (!conVarRef.IsValidRef()) {
		S2_LOGF(LS_WARNING, "Invalid convar handle\n");
		return;
	}

	auto* conVarData = g_pCVar->GetConVarData(conVarRef);
	if (conVarData == nullptr) {
		S2_LOG(LS_WARNING, "Invalid convar handle. Ensure the ConVarRef is correctly initialized and not null.\n");
		return;
	}

	conVarData->RemoveFlags(static_cast<int64>(flags));
}

/**
 * @brief Retrieves the current flags of a console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @return The current flags set on the console variable.
 */
extern "C" PLUGIN_API ConVarFlag GetConVarFlags(uint64_t conVarHandle) {
	ConVarRef conVarRef(conVarHandle);

	if (!conVarRef.IsValidRef()) {
		S2_LOGF(LS_WARNING, "Invalid convar handle\n");
		return ConVarFlag::None;
	}

	auto* conVarData = g_pCVar->GetConVarData(conVarRef);
	if (conVarData == nullptr) {
		S2_LOG(LS_WARNING, "Invalid convar handle. Ensure the ConVarRef is correctly initialized and not null.\n");
		return ConVarFlag::None;
	}

	return static_cast<ConVarFlag>(conVarData->GetFlags());
}

/**
 * @brief Gets the specified bound (max or min) of a console variable and stores it in the output string.
 *
 * @param conVarHandle The handle to the console variable data.
 * @param max Indicates whether to get the maximum (true) or minimum (false) bound.
 * @return The bound value.
 */
extern "C" PLUGIN_API plg::string GetConVarBounds(uint64_t conVarHandle, bool max) {
	ConVarRef conVarRef(conVarHandle);

	if (!conVarRef.IsValidRef()) {
		S2_LOGF(LS_WARNING, "Invalid convar handle\n");
		return {};
	}

	auto* conVarData = g_pCVar->GetConVarData(conVarRef);
	if (conVarData == nullptr) {
		S2_LOG(LS_WARNING, "Invalid convar handle. Ensure the ConVarRef is correctly initialized and not null.\n");
		return {};
	}

	CBufferStringN<512> buffer;

	if (max) {
		conVarData->MaxValueToString(buffer);
	} else {
		conVarData->MinValueToString(buffer);
	}
	return buffer.Get();
}

/**
 * @brief Sets the specified bound (max or min) for a console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @param max Indicates whether to set the maximum (true) or minimum (false) bound.
 * @param value The value to set as the bound.
 */
extern "C" PLUGIN_API void SetConVarBounds(uint64_t conVarHandle, bool max, const plg::string& value) {
	ConVarRef conVarRef(conVarHandle);

	if (!conVarRef.IsValidRef()) {
		S2_LOGF(LS_WARNING, "Invalid convar handle\n");
		return;
	}

	auto* conVarData = g_pCVar->GetConVarData(conVarRef);
	if (conVarData == nullptr) {
		S2_LOG(LS_WARNING, "Invalid convar handle. Ensure the ConVarRef is correctly initialized and not null.\n");
		return;
	}

	if (max) {
		conVarData->UpdateMinValueString(value.c_str());
	} else {
		conVarData->UpdateMaxValueString(value.c_str());
	}
}

/**
 * @brief Retrieves the current default value of a console variable and stores it in the output string.
 *
 * @param conVarHandle The handle to the console variable data.
 * @return The output value in string format.
 */
extern "C" PLUGIN_API plg::string GetConVarDefault(uint64_t conVarHandle) {
	ConVarRef conVarRef(conVarHandle);

	if (!conVarRef.IsValidRef()) {
		S2_LOGF(LS_WARNING, "Invalid convar handle\n");
		return {};
	}

	auto* conVarData = g_pCVar->GetConVarData(conVarRef);
	if (conVarData == nullptr) {
		S2_LOG(LS_WARNING, "Invalid convar handle. Ensure the ConVarRef is correctly initialized and not null.\n");
		return {};
	}

	CBufferStringN<512> buffer;

	conVarData->DefaultValueToString(buffer);

	return buffer.Get();
}

/**
 * @brief Retrieves the current value of a console variable and stores it in the output string.
 *
 * @param conVarHandle The handle to the console variable data.
 * @return The output value in string format.
 */
extern "C" PLUGIN_API plg::string GetConVarValue(uint64_t conVarHandle) {
	ConVarRef conVarRef(conVarHandle);

	if (!conVarRef.IsValidRef()) {
		S2_LOGF(LS_WARNING, "Invalid convar handle\n");
		return {};
	}

	auto* conVarData = g_pCVar->GetConVarData(conVarRef);
	if (conVarData == nullptr) {
		S2_LOG(LS_WARNING, "Invalid convar handle. Ensure the ConVarRef is correctly initialized and not null.\n");
		return {};
	}

	CBufferStringN<512> buffer;

	conVarData->ValueToString(-1, buffer);

	return buffer.Get();
}

/**
 * @brief Retrieves the current value of a console variable and stores it in the output.
 *
 * @param conVarHandle The handle to the console variable data.
 * @return The output value.
 */
extern "C" PLUGIN_API plg::any GetConVar(uint64_t conVarHandle) {
	ConVarRef conVarRef(conVarHandle);

	if (!conVarRef.IsValidRef()) {
		S2_LOGF(LS_WARNING, "Invalid convar handle\n");
		return {};
	}

	auto* conVarData = g_pCVar->GetConVarData(conVarRef);
	if (conVarData == nullptr) {
		S2_LOG(LS_WARNING, "Invalid convar handle. Ensure the ConVarRef is correctly initialized and not null.\n");
		return {};
	}

	return conVarData->ValueOrDefault(-1);
}

/**
 * @brief Retrieves the current value of a boolean console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @return The current boolean value of the console variable.
 */
extern "C" PLUGIN_API bool GetConVarBool(uint64_t conVarHandle) {
	return utils::GetConVarValueByHandle<bool>(conVarHandle);
}

/**
 * @brief Retrieves the current value of a signed 16-bit integer console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @return The current int16 value of the console variable.
 */
extern "C" PLUGIN_API int16 GetConVarInt16(uint64_t conVarHandle) {
	return utils::GetConVarValueByHandle<int16>(conVarHandle);
}

/**
 * @brief Retrieves the current value of an unsigned 16-bit integer console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @return The current uint16 value of the console variable.
 */
extern "C" PLUGIN_API uint16 GetConVarUInt16(uint64_t conVarHandle) {
	return utils::GetConVarValueByHandle<uint16>(conVarHandle);
}

/**
 * @brief Retrieves the current value of a signed 32-bit integer console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @return The current int32 value of the console variable.
 */
extern "C" PLUGIN_API int32 GetConVarInt32(uint64_t conVarHandle) {
	return utils::GetConVarValueByHandle<int32>(conVarHandle);
}

/**
 * @brief Retrieves the current value of an unsigned 32-bit integer console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @return The current uint32 value of the console variable.
 */
extern "C" PLUGIN_API uint32 GetConVarUInt32(uint64_t conVarHandle) {
	return utils::GetConVarValueByHandle<uint32>(conVarHandle);
}

/**
 * @brief Retrieves the current value of a signed 64-bit integer console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @return The current int64 value of the console variable.
 */
extern "C" PLUGIN_API int64 GetConVarInt64(uint64_t conVarHandle) {
	return utils::GetConVarValueByHandle<int64>(conVarHandle);
}

/**
 * @brief Retrieves the current value of an unsigned 64-bit integer console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @return The current uint64 value of the console variable.
 */
extern "C" PLUGIN_API uint64 GetConVarUInt64(uint64_t conVarHandle) {
	return utils::GetConVarValueByHandle<uint64>(conVarHandle);
}

/**
 * @brief Retrieves the current value of a float console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @return The current float value of the console variable.
 */
extern "C" PLUGIN_API float GetConVarFloat(uint64_t conVarHandle) {
	return utils::GetConVarValueByHandle<float>(conVarHandle);
}

/**
 * @brief Retrieves the current value of a double console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @return The current double value of the console variable.
 */
extern "C" PLUGIN_API double GetConVarDouble(uint64_t conVarHandle) {
	return utils::GetConVarValueByHandle<double>(conVarHandle);
}

/**
 * @brief Retrieves the current value of a string console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @return The current string value of the console variable.
 */
extern "C" PLUGIN_API plg::string GetConVarString(uint64_t conVarHandle) {
	return utils::GetConVarValueByHandle<CUtlString>(conVarHandle).Get();
}

/**
 * @brief Retrieves the current value of a Color console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @return The current Color value of the console variable.
 */
extern "C" PLUGIN_API int GetConVarColor(uint64_t conVarHandle) {
	return utils::GetConVarValueByHandle<Color>(conVarHandle).GetRawColor();
}

/**
 * @brief Retrieves the current value of a Vector2D console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @return The current Vector2D value of the console variable.
 */
extern "C" PLUGIN_API plg::vec2 GetConVarVector2(uint64_t conVarHandle) {
	const Vector2D& vec = utils::GetConVarValueByHandle<Vector2D>(conVarHandle);
	return *reinterpret_cast<const plg::vec2*>(&vec);
}

/**
 * @brief Retrieves the current value of a Vector console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @return The current Vector value of the console variable.
 */
extern "C" PLUGIN_API plg::vec3 GetConVarVector(uint64_t conVarHandle) {
	const Vector& vec = utils::GetConVarValueByHandle<Vector>(conVarHandle);
	return *reinterpret_cast<const plg::vec3*>(&vec);
}

/**
 * @brief Retrieves the current value of a Vector4D console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @return The current Vector4D value of the console variable.
 */
extern "C" PLUGIN_API plg::vec4 GetConVarVector4(uint64_t conVarHandle) {
	const Vector4D& vec = utils::GetConVarValueByHandle<Vector4D>(conVarHandle);
	return *reinterpret_cast<const plg::vec4*>(&vec);
}

/**
 * @brief Retrieves the current value of a QAngle console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @return The current QAngle value of the console variable.
 */
extern "C" PLUGIN_API plg::vec3 GetConVarQAngle(uint64_t conVarHandle) {
	const QAngle& ang = utils::GetConVarValueByHandle<QAngle>(conVarHandle);
	return *reinterpret_cast<const plg::vec3*>(&ang);
}

/**
 * @brief Sets the value of a console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @param value The string value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarValue(uint64_t conVarHandle, const plg::string& value, bool replicate, bool notify) {
	utils::SetConVarStringByHandle(conVarHandle, value, replicate, notify);
}

/**
 * @brief Sets the value of a console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVar(uint64_t conVarHandle, const plg::any& value, bool replicate, bool notify) {
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

	switch (conVarData->GetType()) {
		case EConVarType_Bool:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>)
					utils::SetConVar(conVar, static_cast<bool>(v), replicate, notify);
			}, value);
			break;
		case EConVarType_Int16:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>)
					utils::SetConVar(conVar, static_cast<int16>(v), replicate, notify);
			}, value);
			break;
		case EConVarType_UInt16:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>)
					utils::SetConVar(conVar, static_cast<uint16>(v), replicate, notify);
			}, value);
			break;
		case EConVarType_Int32:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>)
					utils::SetConVar(conVar, static_cast<int32>(v), replicate, notify);
			}, value);
			break;
		case EConVarType_UInt32:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>)
					utils::SetConVar(conVar, static_cast<uint32>(v), replicate, notify);
			}, value);
			break;
		case EConVarType_Int64:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>)
					utils::SetConVar(conVar, static_cast<int64>(v), replicate, notify);
			}, value);
			break;
		case EConVarType_UInt64:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>)
					utils::SetConVar(conVar, static_cast<uint64>(v), replicate, notify);
			}, value);
			break;
		case EConVarType_Float32:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>)
					utils::SetConVar(conVar, static_cast<float>(v), replicate, notify);
			}, value);
			break;
		case EConVarType_Float64:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>)
					utils::SetConVar(conVar, static_cast<double>(v), replicate, notify);
			}, value);
			break;
		case EConVarType_String:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_same_v<T, plg::string>)
					utils::SetConVar(conVar, CUtlString(v.c_str()), replicate, notify);
			}, value);
			break;
		case EConVarType_Color:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>) {
					int color = static_cast<int>(v);
					utils::SetConVar(conVar, *reinterpret_cast<Color*>(&color), replicate, notify);
				}
			}, value);
			break;
		case EConVarType_Vector2:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_same_v<T, plg::vec2> || std::is_same_v<T, plg::vec3> || std::is_same_v<T, plg::vec4>)
					utils::SetConVar<Vector2D>(conVar, *reinterpret_cast<const Vector2D*>(&v), replicate, notify);
			}, value);
			break;
		case EConVarType_Vector3:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_same_v<T, plg::vec3> || std::is_same_v<T, plg::vec4>)
					utils::SetConVar<Vector>(conVar, *reinterpret_cast<const Vector*>(&v), replicate, notify);
			}, value);
			break;
		case EConVarType_Vector4:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_same_v<T, plg::vec4>)
					utils::SetConVar<Vector4D>(conVar, *reinterpret_cast<const Vector4D*>(&v), replicate, notify);
			}, value);
			break;
		case EConVarType_Qangle:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_same_v<T, plg::vec3> || std::is_same_v<T, plg::vec4>)
					utils::SetConVar<QAngle>(conVar, *reinterpret_cast<const QAngle*>(&v), replicate, notify);
			}, value);
			break;
		default:
			S2_LOG(LS_WARNING, "Invalid convar type.\n");
			return;
	}
}

/**
 * @brief Sets the value of a boolean console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarBool(uint64_t conVarHandle, bool value, bool replicate, bool notify) {
	utils::SetConVarByHandle(conVarHandle, value, replicate, notify);
}

/**
 * @brief Sets the value of a signed 16-bit integer console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarInt16(uint64_t conVarHandle, int16 value, bool replicate, bool notify) {
	utils::SetConVarByHandle(conVarHandle, value, replicate, notify);
}

/**
 * @brief Sets the value of an unsigned 16-bit integer console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarUInt16(uint64_t conVarHandle, uint16 value, bool replicate, bool notify) {
	utils::SetConVarByHandle(conVarHandle, value, replicate, notify);
}

/**
 * @brief Sets the value of a signed 32-bit integer console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarInt32(uint64_t conVarHandle, int32 value, bool replicate, bool notify) {
	utils::SetConVarByHandle(conVarHandle, value, replicate, notify);
}

/**
 * @brief Sets the value of an unsigned 32-bit integer console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarUInt32(uint64_t conVarHandle, uint32 value, bool replicate, bool notify) {
	utils::SetConVarByHandle(conVarHandle, value, replicate, notify);
}

/**
 * @brief Sets the value of a signed 64-bit integer console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarInt64(uint64_t conVarHandle, int64 value, bool replicate, bool notify) {
	utils::SetConVarByHandle(conVarHandle, value, replicate, notify);
}

/**
 * @brief Sets the value of an unsigned 64-bit integer console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarUInt64(uint64_t conVarHandle, uint64 value, bool replicate, bool notify) {
	utils::SetConVarByHandle(conVarHandle, value, replicate, notify);
}

/**
 * @brief Sets the value of a floating-point console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarFloat(uint64_t conVarHandle, float value, bool replicate, bool notify) {
	utils::SetConVarByHandle(conVarHandle, value, replicate, notify);
}

/**
 * @brief Sets the value of a double-precision floating-point console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarDouble(uint64_t conVarHandle, double value, bool replicate, bool notify) {
	utils::SetConVarByHandle(conVarHandle, value, replicate, notify);
}

/**
 * @brief Sets the value of a string console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarString(uint64_t conVarHandle, const plg::string& value, bool replicate, bool notify) {
	utils::SetConVarByHandle(conVarHandle, CUtlString(value.c_str()), replicate, notify);
}

/**
 * @brief Sets the value of a color console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarColor(uint64_t conVarHandle, int value, bool replicate, bool notify) {
	utils::SetConVarByHandle(conVarHandle, *reinterpret_cast<Color*>(&value), replicate, notify);
}

/**
 * @brief Sets the value of a 2D vector console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarVector2(uint64_t conVarHandle, const Vector2D& value, bool replicate, bool notify) {
	utils::SetConVarByHandle(conVarHandle, value, replicate, notify);
}

/**
 * @brief Sets the value of a 3D vector console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarVector3(uint64_t conVarHandle, const Vector& value, bool replicate, bool notify) {
	utils::SetConVarByHandle(conVarHandle, value, replicate, notify);
}

/**
 * @brief Sets the value of a 4D vector console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarVector4(uint64_t conVarHandle, const Vector4D& value, bool replicate, bool notify) {
	utils::SetConVarByHandle(conVarHandle, value, replicate, notify);
}

/**
 * @brief Sets the value of a quaternion angle console variable.
 *
 * @param conVarHandle The handle to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarQAngle(uint64_t conVarHandle, const QAngle& value, bool replicate, bool notify) {
	utils::SetConVarByHandle(conVarHandle, value, replicate, notify);
}

/**
 * @brief Replicates a console variable value to a specific client. This does not change the actual console variable value.
 *
 * @param playerSlot The index of the player's slot to replicate the value to.
 * @param conVarHandle The handle to the console variable data.
 * @param value The value to send to the client.
 */
extern "C" PLUGIN_API void SendConVarValue(int playerSlot, uint64_t conVarHandle, const plg::string& value) {
	ConVarRef conVarRef(conVarHandle);

	if (!conVarRef.IsValidRef()) {
		S2_LOG(LS_WARNING, "Invalid convar handle.\n");
		return;
	}

	if (!utils::IsPlayerSlot(playerSlot)) {
		S2_LOGF(LS_WARNING, "Cannot execute 'SendConVarValue' on invalid player slot: %d\n", playerSlot);
		return;
	}

	auto* conVarData = g_pCVar->GetConVarData(conVarRef);
	if (conVarData == nullptr) {
		S2_LOG(LS_WARNING, "Invalid convar handle. Ensure the ConVarRef is correctly initialized and not null.\n");
		return;
	}

	utils::SendConVarValue(playerSlot, conVarData->GetName(), value.c_str());
}

/**
 * @brief Retrieves the value of a client's console variable and stores it in the output string.
 *
 * @param playerSlot The index of the player's slot whose console variable value is being retrieved.
 * @param convarName The name of the console variable to retrieve.
 * @return The output string to store the client's console variable value.
 */
extern "C" PLUGIN_API plg::string GetClientConVarValue(int playerSlot, const plg::string& convarName) {
	if (!utils::IsPlayerSlot(playerSlot)) {
		S2_LOGF(LS_WARNING, "Cannot execute 'GetClientConVarValue' on invalid player slot: %d\n", playerSlot);
		return {};
	}

	return g_pEngineServer2->GetClientConVarValue(playerSlot, convarName.c_str());
}

/**
 * @brief Replicates a console variable value to a specific fake client. This does not change the actual console variable value.
 *
 * @param playerSlot The index of the fake client to replicate the value to.
 * @param convarName The name of the console variable.
 * @param convarValue The value to set for the console variable.
 */
extern "C" PLUGIN_API void SetFakeClientConVarValue(int playerSlot, const plg::string& convarName, const plg::string& convarValue) {
	if (!utils::IsPlayerSlot(playerSlot)) {
		S2_LOGF(LS_WARNING, "Cannot execute 'SetFakeClientConVarValue' on invalid player slot: %d\n", playerSlot);
		return;
	}

	g_pEngineServer2->SetFakeClientConVarValue(playerSlot, convarName.c_str(), convarValue.c_str());
}

PLUGIFY_WARN_POP()
