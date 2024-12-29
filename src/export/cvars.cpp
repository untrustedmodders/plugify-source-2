#include <convar.h>
#include <core/con_var_manager.hpp>
#include <core/game_config.hpp>
#include <core/sdk/utils.h>
#include <core/sdk/virtual.h>
#include <plugify/cpp_plugin.hpp>
#include <plugin_export.h>

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
 * @return A pointer to the created console variable.
 */
extern "C" PLUGIN_API BaseConVar* CreateConVar(const plg::string& name, const plg::string& defaultValue, const plg::string& description, int flags) {
	return g_ConVarManager.CreateConVar<const char*>(name, description, defaultValue.data(), flags);
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
 * @return A pointer to the created console variable data.
 */
extern "C" PLUGIN_API BaseConVar* CreateConVarBool(const plg::string& name, bool defaultValue, const plg::string& description, int flags, bool hasMin, bool min, bool hasMax, bool max) {
	return g_ConVarManager.CreateConVar<bool>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

/**
 * @brief Creates a new 16-bit signed integer console variable.
 *
 * This function registers a new console variable of type int16_t with the specified parameters.
 *
 * @param name The name of the console variable.
 * @param defaultValue The default value for the console variable.
 * @param description A brief description of the console variable.
 * @param flags Flags that define the behavior of the console variable.
 * @param hasMin Indicates if a minimum value is provided.
 * @param min The minimum value if hasMin is true.
 * @param hasMax Indicates if a maximum value is provided.
 * @param max The maximum value if hasMax is true.
 * @return A pointer to the created console variable data.
 */
extern "C" PLUGIN_API BaseConVar* CreateConVarInt16(const plg::string& name, int16_t defaultValue, const plg::string& description, int flags, bool hasMin, int16_t min, bool hasMax, int16_t max) {
	return g_ConVarManager.CreateConVar<int16_t>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

/**
 * @brief Creates a new 16-bit unsigned integer console variable.
 *
 * This function registers a new console variable of type uint16_t with the specified parameters.
 *
 * @param name The name of the console variable.
 * @param defaultValue The default value for the console variable.
 * @param description A brief description of the console variable.
 * @param flags Flags that define the behavior of the console variable.
 * @param hasMin Indicates if a minimum value is provided.
 * @param min The minimum value if hasMin is true.
 * @param hasMax Indicates if a maximum value is provided.
 * @param max The maximum value if hasMax is true.
 * @return A pointer to the created console variable data.
 */
extern "C" PLUGIN_API BaseConVar* CreateConVarUInt16(const plg::string& name, uint16_t defaultValue, const plg::string& description, int flags, bool hasMin, uint16_t min, bool hasMax, uint16_t max) {
	return g_ConVarManager.CreateConVar<uint16_t>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

/**
 * @brief Creates a new 32-bit signed integer console variable.
 *
 * This function registers a new console variable of type int32_t with the specified parameters.
 *
 * @param name The name of the console variable.
 * @param defaultValue The default value for the console variable.
 * @param description A brief description of the console variable.
 * @param flags Flags that define the behavior of the console variable.
 * @param hasMin Indicates if a minimum value is provided.
 * @param min The minimum value if hasMin is true.
 * @param hasMax Indicates if a maximum value is provided.
 * @param max The maximum value if hasMax is true.
 * @return A pointer to the created console variable data.
 */
extern "C" PLUGIN_API BaseConVar* CreateConVarInt32(const plg::string& name, int32_t defaultValue, const plg::string& description, int flags, bool hasMin, int32_t min, bool hasMax, int32_t max) {
	return g_ConVarManager.CreateConVar<int32_t>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

/**
 * @brief Creates a new 32-bit unsigned integer console variable.
 *
 * This function registers a new console variable of type uint32_t with the specified parameters.
 *
 * @param name The name of the console variable.
 * @param defaultValue The default value for the console variable.
 * @param description A brief description of the console variable.
 * @param flags Flags that define the behavior of the console variable.
 * @param hasMin Indicates if a minimum value is provided.
 * @param min The minimum value if hasMin is true.
 * @param hasMax Indicates if a maximum value is provided.
 * @param max The maximum value if hasMax is true.
 * @return A pointer to the created console variable data.
 */
extern "C" PLUGIN_API BaseConVar* CreateConVarUInt32(const plg::string& name, uint32_t defaultValue, const plg::string& description, int flags, bool hasMin, uint32_t min, bool hasMax, uint32_t max) {
	return g_ConVarManager.CreateConVar<uint32_t>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}


/**
 * @brief Creates a new 64-bit signed integer console variable.
 *
 * This function registers a new console variable of type int64_t with the specified parameters.
 *
 * @param name The name of the console variable.
 * @param defaultValue The default value for the console variable.
 * @param description A brief description of the console variable.
 * @param flags Flags that define the behavior of the console variable.
 * @param hasMin Indicates if a minimum value is provided.
 * @param min The minimum value if hasMin is true.
 * @param hasMax Indicates if a maximum value is provided.
 * @param max The maximum value if hasMax is true.
 * @return A pointer to the created console variable data.
 */
extern "C" PLUGIN_API BaseConVar* CreateConVarInt64(const plg::string& name, int64_t defaultValue, const plg::string& description, int flags, bool hasMin, int64_t min, bool hasMax, int64_t max) {
	return g_ConVarManager.CreateConVar<int64_t>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

/**
 * @brief Creates a new 64-bit unsigned integer console variable.
 *
 * This function registers a new console variable of type uint64_t with the specified parameters.
 *
 * @param name The name of the console variable.
 * @param defaultValue The default value for the console variable.
 * @param description A brief description of the console variable.
 * @param flags Flags that define the behavior of the console variable.
 * @param hasMin Indicates if a minimum value is provided.
 * @param min The minimum value if hasMin is true.
 * @param hasMax Indicates if a maximum value is provided.
 * @param max The maximum value if hasMax is true.
 * @return A pointer to the created console variable data.
 */
extern "C" PLUGIN_API BaseConVar* CreateConVarUInt64(const plg::string& name, uint64_t defaultValue, const plg::string& description, int flags, bool hasMin, uint64_t min, bool hasMax, uint64_t max) {
	return g_ConVarManager.CreateConVar<uint64_t>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
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
 * @return A pointer to the created console variable data.
 */
extern "C" PLUGIN_API BaseConVar* CreateConVarFloat(const plg::string& name, float defaultValue, const plg::string& description, int flags, bool hasMin, float min, bool hasMax, float max) {
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
 * @return A pointer to the created console variable data.
 */
extern "C" PLUGIN_API BaseConVar* CreateConVarDouble(const plg::string& name, double defaultValue, const plg::string& description, int flags, bool hasMin, double min, bool hasMax, double max) {
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
 * @return A pointer to the created console variable data.
 */
extern "C" PLUGIN_API BaseConVar* CreateConVarColor(const plg::string& name, int defaultValue, const plg::string& description, int flags, bool hasMin, int min, bool hasMax, int max) {
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
 * @return A pointer to the created console variable data.
 */
extern "C" PLUGIN_API BaseConVar* CreateConVarVector2(const plg::string& name, const Vector2D& defaultValue, const plg::string& description, int flags, bool hasMin, const Vector2D& min, bool hasMax, const Vector2D& max) {
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
 * @return A pointer to the created console variable data.
 */
extern "C" PLUGIN_API BaseConVar* CreateConVarVector3(const plg::string& name, const Vector& defaultValue, const plg::string& description, int flags, bool hasMin, const Vector& min, bool hasMax, const Vector& max) {
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
 * @return A pointer to the created console variable data.
 */
extern "C" PLUGIN_API BaseConVar* CreateConVarVector4(const plg::string& name, const Vector4D& defaultValue, const plg::string& description, int flags, bool hasMin, const Vector4D& min, bool hasMax, const Vector4D& max) {
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
 * @return A pointer to the created console variable data.
 */
extern "C" PLUGIN_API BaseConVar* CreateConVarQAngle(const plg::string& name, const QAngle& defaultValue, const plg::string& description, int flags, bool hasMin, const QAngle& min, bool hasMax, const QAngle& max) {
	return g_ConVarManager.CreateConVar<QAngle>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

/**
 * @brief Searches for a console variable. *
 * @param name The name of the console variable to search for.
 * @return Pointer to the console variable data if found; otherwise, nullptr.
 */
extern "C" PLUGIN_API BaseConVar* FindConVar(const plg::string& name) {
	return g_ConVarManager.FindConVar(name);
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
		g_Logger.Log(LS_WARNING, "Invalid callback pointer\n");
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
		g_Logger.Log(LS_WARNING, "Invalid callback pointer\n");
		return;
	}

	g_ConVarManager.UnhookConVarChange(name, callback);
}

/**
 * @brief Checks if a specific flag is set for a console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param flag The flag to check against the console variable.
 * @return True if the flag is set; otherwise, false.
 */
extern "C" PLUGIN_API bool IsConVarFlagSet(BaseConVar* conVar, int64_t flag) {
	if (conVar == nullptr) {
		g_Logger.Log(LS_WARNING, "Invalid convar pointer. Ensure the BaseConVar* is correctly initialized and not null.\n");
		return false;
	}

	return conVar->IsFlagSet(flag);
}

/**
 * @brief Adds flags to a console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param flags The flags to be added.
 */
extern "C" PLUGIN_API void AddConVarFlags(BaseConVar* conVar, int64_t flags) {
	if (conVar == nullptr) {
		g_Logger.Log(LS_WARNING, "Invalid convar pointer. Ensure the BaseConVar* is correctly initialized and not null.\n");
		return;
	}

	conVar->AddFlags(flags);
}

/**
 * @brief Removes flags from a console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param flags The flags to be removed.
 */
extern "C" PLUGIN_API void RemoveConVarFlags(BaseConVar* conVar, int64_t flags) {
	if (conVar == nullptr) {
		g_Logger.Log(LS_WARNING, "Invalid convar pointer. Ensure the BaseConVar* is correctly initialized and not null.\n");
		return;
	}

	conVar->RemoveFlags(flags);
}

/**
 * @brief Retrieves the current flags of a console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current flags set on the console variable.
 */
extern "C" PLUGIN_API int64_t GetConVarFlags(BaseConVar* conVar) {
	if (conVar == nullptr) {
		g_Logger.Log(LS_WARNING, "Invalid convar pointer. Ensure the BaseConVar* is correctly initialized and not null.\n");
		return 0;
	}

	return conVar->GetFlags();
}

/**
 * @brief Gets the specified bound (max or min) of a console variable and stores it in the output string.
 *
 * @param conVar Pointer to the console variable data.
 * @param max Indicates whether to get the maximum (true) or minimum (false) bound.
 * @return The bound value.
 */
extern "C" PLUGIN_API plg::string GetConVarBounds(BaseConVar* conVar, bool max) {
	if (conVar == nullptr) {
		g_Logger.Log(LS_WARNING, "Invalid convar pointer. Ensure the BaseConVar* is correctly initialized and not null.\n");
		return {};
	}

	auto* conVarData = conVar->GetConVarData();
	if (conVarData == nullptr) {
		g_Logger.Log(LS_WARNING, "Invalid convar data. Ensure the conVar has associated data correctly initialized.\n");
		return {};
	}

	plg::string value(512, '\0');
	if (max) {
		switch (conVar->GetType()) {
			case EConVarType_Bool:
				conVarData->Cast<bool>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_Int16:
				conVarData->Cast<int16_t>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_UInt16:
				conVarData->Cast<uint16_t>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_Int32:
				conVarData->Cast<int32_t>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_UInt32:
				conVarData->Cast<uint16_t>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_Int64:
				conVarData->Cast<int64_t>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_UInt64:
				conVarData->Cast<uint64_t>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_Float32:
				conVarData->Cast<float>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_Float64:
				conVarData->Cast<double>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_String:
				conVarData->Cast<const char*>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_Color:
				conVarData->Cast<Color>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_Vector2:
				conVarData->Cast<Vector2D>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_Vector3:
				conVarData->Cast<Vector>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_Vector4:
				conVarData->Cast<Vector4D>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_Qangle:
				conVarData->Cast<QAngle>()->GetStringMaxValue(value.data(), value.size());
				break;
			default:
				g_Logger.Log(LS_WARNING, "Invalid convar type.\n");
				break;
		}
	} else {
		switch (conVar->GetType()) {
			case EConVarType_Bool:
				conVarData->Cast<bool>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_Int16:
				conVarData->Cast<int16_t>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_UInt16:
				conVarData->Cast<uint16_t>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_Int32:
				conVarData->Cast<int32_t>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_UInt32:
				conVarData->Cast<uint16_t>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_Int64:
				conVarData->Cast<int64_t>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_UInt64:
				conVarData->Cast<uint64_t>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_Float32:
				conVarData->Cast<float>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_Float64:
				conVarData->Cast<double>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_String:
				conVarData->Cast<const char*>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_Color:
				conVarData->Cast<Color>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_Vector2:
				conVarData->Cast<Vector2D>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_Vector3:
				conVarData->Cast<Vector>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_Vector4:
				conVarData->Cast<Vector4D>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_Qangle:
				conVarData->Cast<QAngle>()->GetStringMinValue(value.data(), value.size());
				break;
			default:
				g_Logger.Log(LS_WARNING, "Invalid convar type.\n");
				break;
		}
	}
	return value;
}

/**
 * @brief Sets the specified bound (max or min) for a console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param max Indicates whether to set the maximum (true) or minimum (false) bound.
 * @param value The value to set as the bound.
 */
extern "C" PLUGIN_API void SetConVarBounds(BaseConVar* conVar, bool max, const plg::string& value) {
	if (conVar == nullptr) {
		g_Logger.Log(LS_WARNING, "Invalid convar pointer. Ensure the BaseConVar* is correctly initialized and not null.\n");
		return;
	}

	auto* conVarData = conVar->GetConVarData();
	if (conVarData == nullptr) {
		g_Logger.Log(LS_WARNING, "Invalid convar data. Ensure the conVar has associated data correctly initialized.\n");
		return;
	}

	if (max) {
		switch (conVar->GetType()) {
			case EConVarType_Bool:
				conVarData->Cast<bool>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_Int16:
				conVarData->Cast<int16_t>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_UInt16:
				conVarData->Cast<uint16_t>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_Int32:
				conVarData->Cast<int32_t>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_UInt32:
				conVarData->Cast<uint16_t>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_Int64:
				conVarData->Cast<int64_t>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_UInt64:
				conVarData->Cast<uint64_t>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_Float32:
				conVarData->Cast<float>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_Float64:
				conVarData->Cast<double>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_String:
				conVarData->Cast<const char*>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_Color:
				conVarData->Cast<Color>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_Vector2:
				conVarData->Cast<Vector2D>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_Vector3:
				conVarData->Cast<Vector>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_Vector4:
				conVarData->Cast<Vector4D>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_Qangle:
				conVarData->Cast<QAngle>()->SetStringMaxValue(value.c_str());
				break;
			default:
				g_Logger.Log(LS_WARNING, "Invalid convar type.\n");
				break;
		}
	} else {
		switch (conVar->GetType()) {
			case EConVarType_Bool:
				conVarData->Cast<bool>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_Int16:
				conVarData->Cast<int16_t>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_UInt16:
				conVarData->Cast<uint16_t>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_Int32:
				conVarData->Cast<int32_t>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_UInt32:
				conVarData->Cast<uint16_t>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_Int64:
				conVarData->Cast<int64_t>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_UInt64:
				conVarData->Cast<uint64_t>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_Float32:
				conVarData->Cast<float>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_Float64:
				conVarData->Cast<double>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_String:
				conVarData->Cast<const char*>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_Color:
				conVarData->Cast<Color>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_Vector2:
				conVarData->Cast<Vector2D>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_Vector3:
				conVarData->Cast<Vector>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_Vector4:
				conVarData->Cast<Vector4D>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_Qangle:
				conVarData->Cast<QAngle>()->SetStringMinValue(value.c_str());
				break;
			default:
				g_Logger.Log(LS_WARNING, "Invalid convar type.\n");
				break;
		}
	}
}

/**
 * @brief Retrieves the current default value of a console variable and stores it in the output string.
 *
 * @param conVar Pointer to the console variable data.
 * @return The output value in string format.
 */
extern "C" PLUGIN_API plg::string GetConVarDefault(BaseConVar* conVar) {
	if (conVar == nullptr) {
		g_Logger.Log(LS_WARNING, "Invalid convar pointer. Ensure the BaseConVar* is correctly initialized and not null.\n");
		return {};
	}

	auto* conVarData = conVar->GetConVarData();
	if (conVarData == nullptr) {
		g_Logger.Log(LS_WARNING, "Invalid convar data. Ensure the conVar has associated data correctly initialized.\n");
		return {};
	}

	switch (conVarData->GetType()) {
		case EConVarType_Bool:
			return conVarData->Cast<bool>()->GetDefaultValue() ? "true" : "false";
		case EConVarType_Int16:
			return plg::to_string(conVarData->Cast<int16_t>()->GetDefaultValue());
		case EConVarType_UInt16:
			return plg::to_string(conVarData->Cast<uint16_t>()->GetDefaultValue());
		case EConVarType_Int32:
			return plg::to_string(conVarData->Cast<int32_t>()->GetDefaultValue());
		case EConVarType_UInt32:
			return plg::to_string(conVarData->Cast<uint16_t>()->GetDefaultValue());
		case EConVarType_Int64:
			return plg::to_string(conVarData->Cast<int64_t>()->GetDefaultValue());
		case EConVarType_UInt64:
			return plg::to_string(conVarData->Cast<uint64_t>()->GetDefaultValue());
		case EConVarType_Float32:
			return plg::to_string(conVarData->Cast<float>()->GetDefaultValue());
		case EConVarType_Float64:
			return plg::to_string(conVarData->Cast<double>()->GetDefaultValue());
		case EConVarType_String:
			return conVarData->Cast<const char*>()->GetDefaultValue();
		case EConVarType_Color: {
			const auto& value = conVarData->Cast<Color>()->GetDefaultValue();
			return std::format("{} {} {} {}", value.r(), value.g(), value.b(), value.a());
		}
		case EConVarType_Vector2: {
			const auto& value = conVarData->Cast<Vector2D>()->GetDefaultValue();
			return std::format("{} {}", value.x, value.y);
		}
		case EConVarType_Vector3: {
			const auto& value = conVarData->Cast<Vector>()->GetDefaultValue();
			return std::format("{} {} {}", value.x, value.y, value.z);
		}
		case EConVarType_Vector4: {
			const auto& value = conVarData->Cast<Vector4D>()->GetDefaultValue();
			return std::format("{} {} {} {}", value.x, value.y, value.z, value.w);
		}
		case EConVarType_Qangle: {
			const auto& value = conVarData->Cast<QAngle>()->GetDefaultValue();
			return std::format("{} {} {}", value.x, value.y, value.z);
		}
		default: {
			g_Logger.Log(LS_WARNING, "Invalid convar type.\n");
			return {};
		}
	}
}

/**
 * @brief Retrieves the current value of a console variable and stores it in the output string.
 *
 * @param conVar Pointer to the console variable data.
 * @return The output value in string format.
 */
extern "C" PLUGIN_API plg::string GetConVarValue(BaseConVar* conVar) {
	if (conVar == nullptr) {
		g_Logger.Log(LS_WARNING, "Invalid convar pointer. Ensure the BaseConVar* is correctly initialized and not null.\n");
		return {};
	}

	auto* conVarData = conVar->GetConVarData();
	if (conVarData == nullptr) {
		g_Logger.Log(LS_WARNING, "Invalid convar data. Ensure the conVar has associated data correctly initialized.\n");
		return {};
	}

	switch (conVarData->GetType()) {
		case EConVarType_Bool:
			return conVarData->Cast<bool>()->GetValue() ? "true" : "false";
		case EConVarType_Int16:
			return plg::to_string(conVarData->Cast<int16_t>()->GetValue());
		case EConVarType_UInt16:
			return plg::to_string(conVarData->Cast<uint16_t>()->GetValue());
		case EConVarType_Int32:
			return plg::to_string(conVarData->Cast<int32_t>()->GetValue());
		case EConVarType_UInt32:
			return plg::to_string(conVarData->Cast<uint16_t>()->GetValue());
		case EConVarType_Int64:
			return plg::to_string(conVarData->Cast<int64_t>()->GetValue());
		case EConVarType_UInt64:
			return plg::to_string(conVarData->Cast<uint64_t>()->GetValue());
		case EConVarType_Float32:
			return plg::to_string(conVarData->Cast<float>()->GetValue());
		case EConVarType_Float64:
			return plg::to_string(conVarData->Cast<double>()->GetValue());
		case EConVarType_String:
			return conVarData->Cast<const char*>()->GetValue();
		case EConVarType_Color: {
			const auto& value = conVarData->Cast<Color>()->GetValue();
			return std::format("{} {} {} {}", value.r(), value.g(), value.b(), value.a());
		}
		case EConVarType_Vector2: {
			const auto& value = conVarData->Cast<Vector2D>()->GetValue();
			return std::format("{} {}", value.x, value.y);
		}
		case EConVarType_Vector3: {
			const auto& value = conVarData->Cast<Vector>()->GetValue();
			return std::format("{} {} {}", value.x, value.y, value.z);
		}
		case EConVarType_Vector4: {
			const auto& value = conVarData->Cast<Vector4D>()->GetValue();
			return std::format("{} {} {} {}", value.x, value.y, value.z, value.w);
		}
		case EConVarType_Qangle: {
			const auto& value = conVarData->Cast<QAngle>()->GetValue();
			return std::format("{} {} {}", value.x, value.y, value.z);
		}
		default: {
			g_Logger.Log(LS_WARNING, "Invalid convar type.\n");
			return {};
		}
	}
}

/**
 * @brief Retrieves the current value of a console variable and stores it in the output.
 *
 * @param conVar Pointer to the console variable data.
 * @return The output value.
 */
extern "C" PLUGIN_API plg::any GetConVar(BaseConVar* conVar) {
	if (conVar == nullptr) {
		g_Logger.Log(LS_WARNING, "Invalid convar pointer. Ensure the BaseConVar* is correctly initialized and not null.\n");
		return {};
	}

	auto* conVarData = conVar->GetConVarData();
	if (conVarData == nullptr) {
		g_Logger.Log(LS_WARNING, "Invalid convar data. Ensure the conVar has associated data correctly initialized.\n");
		return {};
	}

	switch (conVarData->GetType()) {
		case EConVarType_Bool:
			return conVarData->Cast<bool>()->GetValue();
		case EConVarType_Int16:
			return conVarData->Cast<int16_t>()->GetValue();
		case EConVarType_UInt16:
			return conVarData->Cast<uint16_t>()->GetValue();
		case EConVarType_Int32:
			return conVarData->Cast<int32_t>()->GetValue();
		case EConVarType_UInt32:
			return conVarData->Cast<uint16_t>();
		case EConVarType_Int64:
			return conVarData->Cast<int64_t>()->GetValue();
		case EConVarType_UInt64:
			return conVarData->Cast<uint64_t>()->GetValue();
		case EConVarType_Float32:
			return conVarData->Cast<float>()->GetValue();
		case EConVarType_Float64:
			return conVarData->Cast<double>()->GetValue();
		case EConVarType_String:
			return conVarData->Cast<const char*>()->GetValue();
		case EConVarType_Color: {
			const auto& value = conVarData->Cast<Color>()->GetValue();
			return *reinterpret_cast<const int*>(&value);
		}
		case EConVarType_Vector2: {
			const auto& value = conVarData->Cast<Vector2D>()->GetValue();
			return *reinterpret_cast<const plg::vec2*>(&value);
		}
		case EConVarType_Vector3: {
			const auto& value = conVarData->Cast<Vector>()->GetValue();
			return *reinterpret_cast<const plg::vec3*>(&value);
		}
		case EConVarType_Vector4: {
			const auto& value = conVarData->Cast<Vector4D>()->GetValue();
			return *reinterpret_cast<const plg::vec4*>(&value);
		}
		case EConVarType_Qangle: {
			const auto& value = conVarData->Cast<QAngle>()->GetValue();
			return *reinterpret_cast<const plg::vec3*>(&value);
		}
		default: {
			g_Logger.Log(LS_WARNING, "Invalid convar type.\n");
			return {};
		}
	}
}

/**
 * @brief Retrieves the current value of a boolean console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current boolean value of the console variable.
 */
extern "C" PLUGIN_API bool GetConVarBool(BaseConVar* conVar) {
	return utils::GetConVarValue<bool>(conVar);
}

/**
 * @brief Retrieves the current value of a signed 16-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current int16_t value of the console variable.
 */
extern "C" PLUGIN_API int16_t GetConVarInt16(BaseConVar* conVar) {
	return utils::GetConVarValue<int16_t>(conVar);
}

/**
 * @brief Retrieves the current value of an unsigned 16-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current uint16_t value of the console variable.
 */
extern "C" PLUGIN_API uint16_t GetConVarUInt16(BaseConVar* conVar) {
	return utils::GetConVarValue<uint16_t>(conVar);
}

/**
 * @brief Retrieves the current value of a signed 32-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current int32_t value of the console variable.
 */
extern "C" PLUGIN_API int32_t GetConVarInt32(BaseConVar* conVar) {
	return utils::GetConVarValue<int32_t>(conVar);
}

/**
 * @brief Retrieves the current value of an unsigned 32-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current uint32_t value of the console variable.
 */
extern "C" PLUGIN_API uint32_t GetConVarUInt32(BaseConVar* conVar) {
	return utils::GetConVarValue<uint32_t>(conVar);
}

/**
 * @brief Retrieves the current value of a signed 64-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current int64_t value of the console variable.
 */
extern "C" PLUGIN_API int64_t GetConVarInt64(BaseConVar* conVar) {
	return utils::GetConVarValue<int64_t>(conVar);
}

/**
 * @brief Retrieves the current value of an unsigned 64-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current uint64_t value of the console variable.
 */
extern "C" PLUGIN_API uint64_t GetConVarUInt64(BaseConVar* conVar) {
	return utils::GetConVarValue<uint64_t>(conVar);
}

/**
 * @brief Retrieves the current value of a float console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current float value of the console variable.
 */
extern "C" PLUGIN_API float GetConVarFloat(BaseConVar* conVar) {
	return utils::GetConVarValue<float>(conVar);
}

/**
 * @brief Retrieves the current value of a double console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current double value of the console variable.
 */
extern "C" PLUGIN_API double GetConVarDouble(BaseConVar* conVar) {
	return utils::GetConVarValue<double>(conVar);
}

/**
 * @brief Retrieves the current value of a string console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current string value of the console variable.
 */
extern "C" PLUGIN_API plg::string GetConVarString(BaseConVar* conVar) {
	return utils::GetConVarValue<const char*>(conVar);
}

/**
 * @brief Retrieves the current value of a Color console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current Color value of the console variable.
 */
extern "C" PLUGIN_API int GetConVarColor(BaseConVar* conVar) {
	return utils::GetConVarValue<Color>(conVar).GetRawColor();
}

/**
 * @brief Retrieves the current value of a Vector2D console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current Vector2D value of the console variable.
 */
extern "C" PLUGIN_API plg::vec2 GetConVarVector2(BaseConVar* conVar) {
	const Vector2D& vec = utils::GetConVarValue<Vector2D>(conVar);
	return *reinterpret_cast<const plg::vec2*>(&vec);
}

/**
 * @brief Retrieves the current value of a Vector console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current Vector value of the console variable.
 */
extern "C" PLUGIN_API plg::vec3 GetConVarVector(BaseConVar* conVar) {
	const Vector& vec = utils::GetConVarValue<Vector>(conVar);
	return *reinterpret_cast<const plg::vec3*>(&vec);
}

/**
 * @brief Retrieves the current value of a Vector4D console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current Vector4D value of the console variable.
 */
extern "C" PLUGIN_API plg::vec4 GetConVarVector4(BaseConVar* conVar) {
	const Vector4D& vec = utils::GetConVarValue<Vector4D>(conVar);
	return *reinterpret_cast<const plg::vec4*>(&vec);
}

/**
 * @brief Retrieves the current value of a QAngle console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current QAngle value of the console variable.
 */
extern "C" PLUGIN_API plg::vec3 GetConVarQAngle(BaseConVar* conVar) {
	const QAngle& ang = utils::GetConVarValue<QAngle>(conVar);
	return *reinterpret_cast<const plg::vec3*>(&ang);
}

/**
 * @brief Sets the value of a console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The string value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarValue(BaseConVar* conVar, const plg::string& value, bool replicate, bool notify) {
	switch (conVar->GetType()) {
		case EConVarType_Bool:
			utils::SetConVarString<bool>(conVar, value, replicate, notify);
			break;
		case EConVarType_Int16:
			utils::SetConVarString<int16_t>(conVar, value, replicate, notify);
			break;
		case EConVarType_UInt16:
			utils::SetConVarString<uint16_t>(conVar, value, replicate, notify);
			break;
		case EConVarType_Int32:
			utils::SetConVarString<int32_t>(conVar, value, replicate, notify);
			break;
		case EConVarType_UInt32:
			utils::SetConVarString<uint32_t>(conVar, value, replicate, notify);
			break;
		case EConVarType_Int64:
			utils::SetConVarString<int64_t>(conVar, value, replicate, notify);
			break;
		case EConVarType_UInt64:
			utils::SetConVarString<uint64_t>(conVar, value, replicate, notify);
			break;
		case EConVarType_Float32:
			utils::SetConVarString<float>(conVar, value, replicate, notify);
			break;
		case EConVarType_Float64:
			utils::SetConVarString<double>(conVar, value, replicate, notify);
			break;
		case EConVarType_String:
			utils::SetConVarString<const char*>(conVar, value, replicate, notify);
			break;
		case EConVarType_Color:
			utils::SetConVarString<Color>(conVar, value, replicate, notify);
			break;
		case EConVarType_Vector2:
			utils::SetConVarString<Vector2D>(conVar, value, replicate, notify);
			break;
		case EConVarType_Vector3:
			utils::SetConVarString<Vector>(conVar, value, replicate, notify);
			break;
		case EConVarType_Vector4:
			utils::SetConVarString<Vector4D>(conVar, value, replicate, notify);
			break;
		case EConVarType_Qangle:
			utils::SetConVarString<QAngle>(conVar, value, replicate, notify);
			break;
		default:
			g_Logger.Log(LS_WARNING, "Invalid convar type.\n");
			return;
	}
}

/**
 * @brief Sets the value of a console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVar(BaseConVar* conVar, const plg::any& value, bool replicate, bool notify) {
	switch (conVar->GetType()) {
		case EConVarType_Bool:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>)
					utils::SetConVar<bool>(conVar, static_cast<bool>(v), replicate, notify);
			}, value);
			break;
		case EConVarType_Int16:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>)
					utils::SetConVar<int16_t>(conVar, static_cast<int16_t>(v), replicate, notify);
			}, value);
			break;
		case EConVarType_UInt16:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>)
					utils::SetConVar<uint16_t>(conVar, static_cast<uint16_t>(v), replicate, notify);
			}, value);
			break;
		case EConVarType_Int32:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>)
					utils::SetConVar<int32_t>(conVar, static_cast<int32_t>(v), replicate, notify);
			}, value);
			break;
		case EConVarType_UInt32:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>)
					utils::SetConVar<uint32_t>(conVar, static_cast<uint32_t>(v), replicate, notify);
			}, value);
			break;
		case EConVarType_Int64:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>)
					utils::SetConVar<int64_t>(conVar, static_cast<int64_t>(v), replicate, notify);
			}, value);
			break;
		case EConVarType_UInt64:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>)
					utils::SetConVar<uint64_t>(conVar, static_cast<uint64_t>(v), replicate, notify);
			}, value);
			break;
		case EConVarType_Float32:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>)
					utils::SetConVar<float>(conVar, static_cast<float>(v), replicate, notify);
			}, value);
			break;
		case EConVarType_Float64:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>)
					utils::SetConVar<double>(conVar, static_cast<double>(v), replicate, notify);
			}, value);
			break;
		case EConVarType_String:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_same_v<T, plg::string>)
					utils::SetConVar<const char*>(conVar, v.c_str(), replicate, notify);
			}, value);
			break;
		case EConVarType_Color:
			plg::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_arithmetic_v<T>) {
					int color = static_cast<int>(v);
					utils::SetConVar<Color>(conVar, *reinterpret_cast<Color*>(&color), replicate, notify);
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
			g_Logger.Log(LS_WARNING, "Invalid convar type.\n");
			return;
	}
}

/**
 * @brief Sets the value of a boolean console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarBool(BaseConVar* conVar, bool value, bool replicate, bool notify) {
	utils::SetConVar(conVar, value, replicate, notify);
}

/**
 * @brief Sets the value of a signed 16-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarInt16(BaseConVar* conVar, int16_t value, bool replicate, bool notify) {
	utils::SetConVar(conVar, value, replicate, notify);
}

/**
 * @brief Sets the value of an unsigned 16-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarUInt16(BaseConVar* conVar, uint16_t value, bool replicate, bool notify) {
	utils::SetConVar(conVar, value, replicate, notify);
}

/**
 * @brief Sets the value of a signed 32-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarInt32(BaseConVar* conVar, int32_t value, bool replicate, bool notify) {
	utils::SetConVar(conVar, value, replicate, notify);
}

/**
 * @brief Sets the value of an unsigned 32-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarUInt32(BaseConVar* conVar, uint32_t value, bool replicate, bool notify) {
	utils::SetConVar(conVar, value, replicate, notify);
}

/**
 * @brief Sets the value of a signed 64-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarInt64(BaseConVar* conVar, int64_t value, bool replicate, bool notify) {
	utils::SetConVar(conVar, value, replicate, notify);
}

/**
 * @brief Sets the value of an unsigned 64-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarUInt64(BaseConVar* conVar, uint64_t value, bool replicate, bool notify) {
	utils::SetConVar(conVar, value, replicate, notify);
}

/**
 * @brief Sets the value of a floating-point console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarFloat(BaseConVar* conVar, float value, bool replicate, bool notify) {
	utils::SetConVar(conVar, value, replicate, notify);
}

/**
 * @brief Sets the value of a double-precision floating-point console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarDouble(BaseConVar* conVar, double value, bool replicate, bool notify) {
	utils::SetConVar(conVar, value, replicate, notify);
}

/**
 * @brief Sets the value of a string console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarString(BaseConVar* conVar, const plg::string& value, bool replicate, bool notify) {
	utils::SetConVar(conVar, value.c_str(), replicate, notify);
}

/**
 * @brief Sets the value of a color console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarColor(BaseConVar* conVar, int value, bool replicate, bool notify) {
	utils::SetConVar(conVar, *reinterpret_cast<Color*>(&value), replicate, notify);
}

/**
 * @brief Sets the value of a 2D vector console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarVector2(BaseConVar* conVar, const Vector2D& value, bool replicate, bool notify) {
	utils::SetConVar(conVar, value, replicate, notify);
}

/**
 * @brief Sets the value of a 3D vector console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarVector3(BaseConVar* conVar, const Vector& value, bool replicate, bool notify) {
	utils::SetConVar(conVar, value, replicate, notify);
}

/**
 * @brief Sets the value of a 4D vector console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarVector4(BaseConVar* conVar, const Vector4D& value, bool replicate, bool notify) {
	utils::SetConVar(conVar, value, replicate, notify);
}

/**
 * @brief Sets the value of a quaternion angle console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 * @param replicate If set to true, the new convar value will be set on all clients. This will only work if the convar has the FCVAR_REPLICATED flag and actually exists on clients.
 * @param notify If set to true, clients will be notified that the convar has changed. This will only work if the convar has the FCVAR_NOTIFY flag.
 */
extern "C" PLUGIN_API void SetConVarQAngle(BaseConVar* conVar, const QAngle& value, bool replicate, bool notify) {
	utils::SetConVar(conVar, value, replicate, notify);
}

/**
 * @brief Replicates a console variable value to a specific client. This does not change the actual console variable value.
 *
 * @param conVar Pointer to the console variable data.
 * @param clientIndex The index of the client to replicate the value to.
 * @param value The value to send to the client.
 */
extern "C" PLUGIN_API void SendConVarValue(BaseConVar* conVar, int clientIndex, const plg::string& value) {
	utils::SendConVarValue(CPlayerSlot(clientIndex), conVar, value.c_str());
}

/**
 * @brief Retrieves the value of a client's console variable and stores it in the output string.
 *
 * @param clientIndex The index of the client whose console variable value is being retrieved.
 * @param convarName The name of the console variable to retrieve.
 * @return The output string to store the client's console variable value.
 */
extern "C" PLUGIN_API plg::string GetClientConVarValue(int clientIndex, const plg::string& convarName) {
	return g_pEngineServer2->GetClientConVarValue(CPlayerSlot(clientIndex), convarName.c_str());
}

/**
 * @brief Replicates a console variable value to a specific fake client. This does not change the actual console variable value.
 *
 * @param clientIndex The index of the fake client to replicate the value to.
 * @param convarName The name of the console variable.
 * @param convarValue The value to set for the console variable.
 */
extern "C" PLUGIN_API void SetFakeClientConVarValue(int clientIndex, const plg::string& convarName, const plg::string& convarValue) {
	g_pEngineServer2->SetFakeClientConVarValue(CPlayerSlot(clientIndex), convarName.c_str(), convarValue.c_str());
}

PLUGIFY_WARN_POP()
