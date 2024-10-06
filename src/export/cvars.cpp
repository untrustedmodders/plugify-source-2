#include <convar.h>
#include <core/con_var_manager.h>
#include <core/game_config.h>
#include <core/sdk/utils.h>
#include <core/sdk/virtual.h>
#include <plugify/cpp_plugin.h>
#include <plugin_export.h>

// CreateConVar()

/**
 * @brief Creates a new console variable.
 * @param name The name of the console variable.
 * @param defaultValue The default value of the console variable.
 * @param description A description of the console variable's purpose.
 * @param flags Additional flags for the console variable.
 * @return A pointer to the created console variable.
 */
extern "C" PLUGIN_API CConVarBaseData* CreateConVar(const plg::string& name, const plg::string& defaultValue, const plg::string& description, int flags)
{
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
extern "C" PLUGIN_API CConVarBaseData* CreateConVarBool(const plg::string& name, bool defaultValue, const plg::string& description, int flags, bool hasMin, bool min, bool hasMax, bool max)
{
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
extern "C" PLUGIN_API CConVarBaseData* CreateConVarInt16(const plg::string& name, int16_t defaultValue, const plg::string& description, int flags, bool hasMin, int16_t min, bool hasMax, int16_t max)
{
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
extern "C" PLUGIN_API CConVarBaseData* CreateConVarUInt16(const plg::string& name, uint16_t defaultValue, const plg::string& description, int flags, bool hasMin, uint16_t min, bool hasMax, uint16_t max)
{
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
extern "C" PLUGIN_API CConVarBaseData* CreateConVarInt32(const plg::string& name, int32_t defaultValue, const plg::string& description, int flags, bool hasMin, int32_t min, bool hasMax, int32_t max)
{
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
extern "C" PLUGIN_API CConVarBaseData* CreateConVarUInt32(const plg::string& name, uint32_t defaultValue, const plg::string& description, int flags, bool hasMin, uint32_t min, bool hasMax, uint32_t max)
{
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
extern "C" PLUGIN_API CConVarBaseData* CreateConVarInt64(const plg::string& name, int64_t defaultValue, const plg::string& description, int flags, bool hasMin, int64_t min, bool hasMax, int64_t max)
{
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
extern "C" PLUGIN_API CConVarBaseData* CreateConVarUInt64(const plg::string& name, uint64_t defaultValue, const plg::string& description, int flags, bool hasMin, uint64_t min, bool hasMax, uint64_t max)
{
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
extern "C" PLUGIN_API CConVarBaseData* CreateConVarFloat(const plg::string& name, float defaultValue, const plg::string& description, int flags, bool hasMin, float min, bool hasMax, float max)
{
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
extern "C" PLUGIN_API CConVarBaseData* CreateConVarDouble(const plg::string& name, double defaultValue, const plg::string& description, int flags, bool hasMin, double min, bool hasMax, double max)
{
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
extern "C" PLUGIN_API CConVarBaseData* CreateConVarColor(const plg::string& name, int defaultValue, const plg::string& description, int flags, bool hasMin, int min, bool hasMax, int max)
{
	Color defaultCol;
	defaultCol.SetRawColor(defaultValue);
	Color minCol;
	defaultCol.SetRawColor(min);
	Color maxCol;
	defaultCol.SetRawColor(max);
	return g_ConVarManager.CreateConVar<Color>(name, description, defaultCol, flags, hasMin, minCol, hasMax, maxCol);
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
extern "C" PLUGIN_API CConVarBaseData* CreateConVarVector2(const plg::string& name, const Vector2D& defaultValue, const plg::string& description, int flags, bool hasMin, const Vector2D& min, bool hasMax, const Vector2D& max)
{
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
extern "C" PLUGIN_API CConVarBaseData* CreateConVarVector3(const plg::string& name, const Vector& defaultValue, const plg::string& description, int flags, bool hasMin, const Vector& min, bool hasMax, const Vector& max)
{
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
extern "C" PLUGIN_API CConVarBaseData* CreateConVarVector4(const plg::string& name, const Vector4D& defaultValue, const plg::string& description, int flags, bool hasMin, const Vector4D& min, bool hasMax, const Vector4D& max)
{
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
extern "C" PLUGIN_API CConVarBaseData* CreateConVarQangle(const plg::string& name, const QAngle& defaultValue, const plg::string& description, int flags, bool hasMin, const QAngle& min, bool hasMax, const QAngle& max)
{
	return g_ConVarManager.CreateConVar<QAngle>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

/**
 * @brief Searches for a console variable. *
 * @param name The name of the console variable to search for.
 * @return Pointer to the console variable data if found; otherwise, nullptr.
 */
extern "C" PLUGIN_API CConVarBaseData* FindConVar(const plg::string& name)
{
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
extern "C" PLUGIN_API void HookConVarChange(const plg::string& name, ConVarChangeListenerCallback callback)
{
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
extern "C" PLUGIN_API void UnhookConVarChange(const plg::string& name, ConVarChangeListenerCallback callback)
{
	g_ConVarManager.UnhookConVarChange(name, callback);
}

/**
 * @brief Checks if a specific flag is set for a console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param flag The flag to check against the console variable.
 * @return True if the flag is set; otherwise, false.
 */
extern "C" PLUGIN_API bool IsConVarFlagSet(CConVarBaseData* conVar, int64_t flag)
{
	return conVar->IsFlagSet(flag);
}

/**
 * @brief Adds flags to a console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param flags The flags to be added.
 */
extern "C" PLUGIN_API void AddConVarFlags(CConVarBaseData* conVar, int64_t flags)
{
	conVar->AddFlags(flags);
}

/**
 * @brief Removes flags from a console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param flags The flags to be removed.
 */
extern "C" PLUGIN_API void RemoveConVarFlags(CConVarBaseData* conVar, int64_t flags)
{
	conVar->RemoveFlags(flags);
}

/**
 * @brief Retrieves the current flags of a console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current flags set on the console variable.
 */
extern "C" PLUGIN_API int64_t GetConVarFlags(CConVarBaseData* conVar)
{
	return conVar->GetFlags();
}

/**
 * @brief Gets the specified bound (max or min) of a console variable and stores it in the output string.
 *
 * @param output Reference to the output string to store the bound value.
 * @param conVar Pointer to the console variable data.
 * @param max Indicates whether to get the maximum (true) or minimum (false) bound.
 */
extern "C" PLUGIN_API void GetConVarBounds(const plg::string& output, CConVarBaseData* conVar, bool max)
{
	plg::string value(256, '\0');
	if (max)
	{
		switch (conVar->GetType())
		{
			case EConVarType_Bool:
				conVar->Cast<bool>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_Int16:
				conVar->Cast<int16_t>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_UInt16:
				conVar->Cast<uint16_t>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_Int32:
				conVar->Cast<int32_t>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_UInt32:
				conVar->Cast<uint16_t>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_Int64:
				conVar->Cast<int64_t>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_UInt64:
				conVar->Cast<uint64_t>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_Float32:
				conVar->Cast<float>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_Float64:
				conVar->Cast<double>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_String:
				conVar->Cast<const char*>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_Color:
				conVar->Cast<Color>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_Vector2:
				conVar->Cast<Vector2D>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_Vector3:
				conVar->Cast<Vector>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_Vector4:
				conVar->Cast<Vector4D>()->GetStringMaxValue(value.data(), value.size());
				break;
			case EConVarType_Qangle:
				conVar->Cast<QAngle>()->GetStringMaxValue(value.data(), value.size());
				break;
			default:
				g_Logger.Log(LS_WARNING, "Invalid convar type.\n");
				break;
		}
	}
	else
	{
		switch (conVar->GetType())
		{
			case EConVarType_Bool:
				conVar->Cast<bool>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_Int16:
				conVar->Cast<int16_t>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_UInt16:
				conVar->Cast<uint16_t>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_Int32:
				conVar->Cast<int32_t>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_UInt32:
				conVar->Cast<uint16_t>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_Int64:
				conVar->Cast<int64_t>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_UInt64:
				conVar->Cast<uint64_t>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_Float32:
				conVar->Cast<float>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_Float64:
				conVar->Cast<double>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_String:
				conVar->Cast<const char*>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_Color:
				conVar->Cast<Color>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_Vector2:
				conVar->Cast<Vector2D>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_Vector3:
				conVar->Cast<Vector>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_Vector4:
				conVar->Cast<Vector4D>()->GetStringMinValue(value.data(), value.size());
				break;
			case EConVarType_Qangle:
				conVar->Cast<QAngle>()->GetStringMinValue(value.data(), value.size());
				break;
			default:
				g_Logger.Log(LS_WARNING, "Invalid convar type.\n");
				break;
		}
	}
	std::construct_at(&output, std::move(value));
}

/**
 * @brief Sets the specified bound (max or min) for a console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param max Indicates whether to set the maximum (true) or minimum (false) bound.
 * @param value The value to set as the bound.
 */
extern "C" PLUGIN_API void SetConVarBounds(CConVarBaseData* conVar, bool max, const plg::string& value)
{
	if (max)
	{
		switch (conVar->GetType())
		{
			case EConVarType_Bool:
				conVar->Cast<bool>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_Int16:
				conVar->Cast<int16_t>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_UInt16:
				conVar->Cast<uint16_t>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_Int32:
				conVar->Cast<int32_t>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_UInt32:
				conVar->Cast<uint16_t>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_Int64:
				conVar->Cast<int64_t>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_UInt64:
				conVar->Cast<uint64_t>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_Float32:
				conVar->Cast<float>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_Float64:
				conVar->Cast<double>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_String:
				conVar->Cast<const char*>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_Color:
				conVar->Cast<Color>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_Vector2:
				conVar->Cast<Vector2D>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_Vector3:
				conVar->Cast<Vector>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_Vector4:
				conVar->Cast<Vector4D>()->SetStringMaxValue(value.c_str());
				break;
			case EConVarType_Qangle:
				conVar->Cast<QAngle>()->SetStringMaxValue(value.c_str());
				break;
			default:
				g_Logger.Log(LS_WARNING, "Invalid convar type.\n");
				break;
		}
	}
	else
	{
		switch (conVar->GetType())
		{
			case EConVarType_Bool:
				conVar->Cast<bool>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_Int16:
				conVar->Cast<int16_t>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_UInt16:
				conVar->Cast<uint16_t>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_Int32:
				conVar->Cast<int32_t>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_UInt32:
				conVar->Cast<uint16_t>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_Int64:
				conVar->Cast<int64_t>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_UInt64:
				conVar->Cast<uint64_t>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_Float32:
				conVar->Cast<float>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_Float64:
				conVar->Cast<double>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_String:
				conVar->Cast<const char*>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_Color:
				conVar->Cast<Color>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_Vector2:
				conVar->Cast<Vector2D>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_Vector3:
				conVar->Cast<Vector>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_Vector4:
				conVar->Cast<Vector4D>()->SetStringMinValue(value.c_str());
				break;
			case EConVarType_Qangle:
				conVar->Cast<QAngle>()->SetStringMinValue(value.c_str());
				break;
			default:
				g_Logger.Log(LS_WARNING, "Invalid convar type.\n");
				break;
		}
	}
}

/**
 * @brief Retrieves the current value of a console variable and stores it in the output string.
 *
 * @param conVar Pointer to the console variable data.
 * @return The output value in string format.
 */
extern "C" PLUGIN_API void GetConVarValue(plg::string& output, CConVarBaseData* conVar)
{
	switch (conVar->GetType())
	{
		case EConVarType_Bool:
			std::construct_at(&output, conVar->Cast<bool>()->GetValue() ? "1" : "0");
			break;
		case EConVarType_Int16:
			std::construct_at(&output, std::to_string(conVar->Cast<int16_t>()->GetValue()));
			break;
		case EConVarType_UInt16:
			std::construct_at(&output, std::to_string(conVar->Cast<uint16_t>()->GetValue()));
			break;
		case EConVarType_Int32:
			std::construct_at(&output, std::to_string(conVar->Cast<int32_t>()->GetValue()));
			break;
		case EConVarType_UInt32:
			std::construct_at(&output, std::to_string(conVar->Cast<uint16_t>()->GetValue()));
			break;
		case EConVarType_Int64:
			std::construct_at(&output, std::to_string(conVar->Cast<int64_t>()->GetValue()));
			break;
		case EConVarType_UInt64:
			std::construct_at(&output, std::to_string(conVar->Cast<uint64_t>()->GetValue()));
			break;
		case EConVarType_Float32:
			std::construct_at(&output, std::to_string(conVar->Cast<float>()->GetValue()));
			break;
		case EConVarType_Float64:
			std::construct_at(&output, std::to_string(conVar->Cast<double>()->GetValue()));
			break;
		case EConVarType_String:
			std::construct_at(&output, conVar->Cast<const char*>()->GetValue());
			break;
		case EConVarType_Color:
		{
			const auto& value = conVar->Cast<Color>()->GetValue();
			std::construct_at(&output, std::format("{} {} {} {}", value.r(), value.g(), value.b(), value.a()));
			break;
		}
		case EConVarType_Vector2:
		{
			const auto& value = conVar->Cast<Vector2D>()->GetValue();
			std::construct_at(&output, std::format("{} {}", value.x, value.y));
			break;
		}
		case EConVarType_Vector3:
		{
			const auto& value = conVar->Cast<Vector>()->GetValue();
			std::construct_at(&output, std::format("{} {} {}", value.x, value.y, value.z));
			break;
		}
		case EConVarType_Vector4:
		{
			const auto& value = conVar->Cast<Vector4D>()->GetValue();
			std::construct_at(&output, std::format("{} {} {} {}", value.x, value.y, value.z, value.w));
			break;
		}
		case EConVarType_Qangle:
		{
			const auto& value = conVar->Cast<QAngle>()->GetValue();
			std::construct_at(&output, std::format("{} {} {}", value.x, value.y, value.z));
			break;
		}
		default:
			g_Logger.Log(LS_WARNING, "Invalid convar type.\n");
			break;
	}
}

/**
 * @brief Retrieves the current value of a boolean console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current boolean value of the console variable.
 */
extern "C" PLUGIN_API bool GetConVarBool(CConVarBaseData* conVar)
{
    return conVar->Cast<bool>()->GetValue();
}

/**
 * @brief Retrieves the current value of a signed 16-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current int16_t value of the console variable.
 */
extern "C" PLUGIN_API int16_t GetConVarInt16(CConVarBaseData* conVar)
{
    return conVar->Cast<int16_t>()->GetValue();
}

/**
 * @brief Retrieves the current value of an unsigned 16-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current uint16_t value of the console variable.
 */
extern "C" PLUGIN_API uint16_t GetConVarUInt16(CConVarBaseData* conVar)
{
    return conVar->Cast<uint16_t>()->GetValue();
}

/**
 * @brief Retrieves the current value of a signed 32-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current int32_t value of the console variable.
 */
extern "C" PLUGIN_API int32_t GetConVarInt32(CConVarBaseData* conVar)
{
    return conVar->Cast<int32_t>()->GetValue();
}

/**
 * @brief Retrieves the current value of an unsigned 32-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current uint32_t value of the console variable.
 */
extern "C" PLUGIN_API uint32_t GetConVarUInt32(CConVarBaseData* conVar)
{
    return conVar->Cast<uint32_t>()->GetValue();
}

/**
 * @brief Retrieves the current value of a signed 64-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current int64_t value of the console variable.
 */
extern "C" PLUGIN_API int64_t GetConVarInt64(CConVarBaseData* conVar)
{
    return conVar->Cast<int64_t>()->GetValue();
}

/**
 * @brief Retrieves the current value of an unsigned 64-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current uint64_t value of the console variable.
 */
extern "C" PLUGIN_API uint64_t GetConVarUInt64(CConVarBaseData* conVar)
{
    return conVar->Cast<uint64_t>()->GetValue();
}

/**
 * @brief Retrieves the current value of a float console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current float value of the console variable.
 */
extern "C" PLUGIN_API float GetConVarFloat(CConVarBaseData* conVar)
{
    return conVar->Cast<float>()->GetValue();
}

/**
 * @brief Retrieves the current value of a double console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current double value of the console variable.
 */
extern "C" PLUGIN_API double GetConVarDouble(CConVarBaseData* conVar)
{
    return conVar->Cast<double>()->GetValue();
}

/**
 * @brief Retrieves the current value of a string console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current string value of the console variable.
 */
extern "C" PLUGIN_API void GetConVarString(plg::string& output, CConVarBaseData* conVar)
{
	std::construct_at(&output, conVar->Cast<const char*>()->GetValue());
}

/**
 * @brief Retrieves the current value of a Color console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current Color value of the console variable.
 */
extern "C" PLUGIN_API Color GetConVarColor(CConVarBaseData* conVar)
{
    return conVar->Cast<Color>()->GetValue();
}

/**
 * @brief Retrieves the current value of a Vector2D console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current Vector2D value of the console variable.
 */
extern "C" PLUGIN_API plg::vec2 GetConVarVector2(CConVarBaseData* conVar)
{
	const Vector2D& vec = conVar->Cast<Vector2D>()->GetValue();
    return *reinterpret_cast<const plg::vec2*>(&vec);
}

/**
 * @brief Retrieves the current value of a Vector console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current Vector value of the console variable.
 */
extern "C" PLUGIN_API plg::vec3 GetConVarVector(CConVarBaseData* conVar)
{
	const Vector& vec = conVar->Cast<Vector>()->GetValue();
    return *reinterpret_cast<const plg::vec3*>(&vec);
}

/**
 * @brief Retrieves the current value of a Vector4D console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current Vector4D value of the console variable.
 */
extern "C" PLUGIN_API plg::vec4 GetConVarVector4(CConVarBaseData* conVar)
{
	const Vector4D& vec = conVar->Cast<Vector4D>()->GetValue();
    return *reinterpret_cast<const plg::vec4*>(&vec);
}

/**
 * @brief Retrieves the current value of a QAngle console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @return The current QAngle value of the console variable.
 */
extern "C" PLUGIN_API plg::vec3 GetConVarQangle(CConVarBaseData* conVar)
{
	const QAngle& ang = conVar->Cast<QAngle>()->GetValue();
    return *reinterpret_cast<const plg::vec3*>(&ang);
}

/**
 * @brief Sets the value of a console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 */
extern "C" PLUGIN_API void SetConVarValue(CConVarBaseData* conVar, const plg::string& value)
{
	switch (conVar->GetType())
	{
		case EConVarType_Bool:
			conVar->Cast<bool>()->SetStringValue(value.c_str());
			break;
		case EConVarType_Int16:
			conVar->Cast<int16_t>()->SetStringValue(value.c_str());
			break;
		case EConVarType_UInt16:
			conVar->Cast<uint16_t>()->SetStringValue(value.c_str());
			break;
		case EConVarType_Int32:
			conVar->Cast<int32_t>()->SetStringValue(value.c_str());
			break;
		case EConVarType_UInt32:
			conVar->Cast<uint16_t>()->SetStringValue(value.c_str());
			break;
		case EConVarType_Int64:
			conVar->Cast<int64_t>()->SetStringValue(value.c_str());
			break;
		case EConVarType_UInt64:
			conVar->Cast<uint64_t>()->SetStringValue(value.c_str());
			break;
		case EConVarType_Float32:
			conVar->Cast<float>()->SetStringValue(value.c_str());
			break;
		case EConVarType_Float64:
			conVar->Cast<double>()->SetStringValue(value.c_str());
			break;
		case EConVarType_String:
			conVar->Cast<const char*>()->SetStringValue(value.c_str());
			break;
		case EConVarType_Color:
			conVar->Cast<Color>()->SetStringValue(value.c_str());
			break;
		case EConVarType_Vector2:
			conVar->Cast<Vector2D>()->SetStringValue(value.c_str());
			break;
		case EConVarType_Vector3:
			conVar->Cast<Vector>()->SetStringValue(value.c_str());
			break;
		case EConVarType_Vector4:
			conVar->Cast<Vector4D>()->SetStringValue(value.c_str());
			break;
		case EConVarType_Qangle:
			conVar->Cast<QAngle>()->SetStringValue(value.c_str());
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
 */
extern "C" PLUGIN_API void SetConVarBool(CConVarBaseData* conVar, bool value)
{
    conVar->Cast<bool>()->SetValue(value);
}

/**
 * @brief Sets the value of a signed 16-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 */
extern "C" PLUGIN_API void SetConVarInt16(CConVarBaseData* conVar, int16_t value)
{
    conVar->Cast<int16_t>()->SetValue(value);
}

/**
 * @brief Sets the value of an unsigned 16-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 */
extern "C" PLUGIN_API void SetConVarUInt16(CConVarBaseData* conVar, uint16_t value)
{
    conVar->Cast<uint16_t>()->SetValue(value);
}

/**
 * @brief Sets the value of a signed 32-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 */
extern "C" PLUGIN_API void SetConVarInt32(CConVarBaseData* conVar, int32_t value)
{
    conVar->Cast<int32_t>()->SetValue(value);
}

/**
 * @brief Sets the value of an unsigned 32-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 */
extern "C" PLUGIN_API void SetConVarUInt32(CConVarBaseData* conVar, uint32_t value)
{
    conVar->Cast<uint32_t>()->SetValue(value);
}

/**
 * @brief Sets the value of a signed 64-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 */
extern "C" PLUGIN_API void SetConVarInt64(CConVarBaseData* conVar, int64_t value)
{
    conVar->Cast<int64_t>()->SetValue(value);
}

/**
 * @brief Sets the value of an unsigned 64-bit integer console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 */
extern "C" PLUGIN_API void SetConVarUInt64(CConVarBaseData* conVar, uint64_t value)
{
    conVar->Cast<uint64_t>()->SetValue(value);
}

/**
 * @brief Sets the value of a floating-point console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 */
extern "C" PLUGIN_API void SetConVarFloat(CConVarBaseData* conVar, float value)
{
    conVar->Cast<float>()->SetValue(value);
}

/**
 * @brief Sets the value of a double-precision floating-point console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 */
extern "C" PLUGIN_API void SetConVarDouble(CConVarBaseData* conVar, double value)
{
    conVar->Cast<double>()->SetValue(value);
}

/**
 * @brief Sets the value of a string console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 */
extern "C" PLUGIN_API void SetConVarString(CConVarBaseData* conVar, const plg::string& value)
{
    conVar->Cast<const char*>()->SetValue(value.c_str());
}

/**
 * @brief Sets the value of a color console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 */
extern "C" PLUGIN_API void SetConVarColor(CConVarBaseData* conVar, const Color& value)
{
    conVar->Cast<Color>()->SetValue(value);
}

/**
 * @brief Sets the value of a 2D vector console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 */
extern "C" PLUGIN_API void SetConVarVector2(CConVarBaseData* conVar, const Vector2D& value)
{
    conVar->Cast<Vector2D>()->SetValue(value);
}

/**
 * @brief Sets the value of a 3D vector console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 */
extern "C" PLUGIN_API void SetConVarVector3(CConVarBaseData* conVar, const Vector& value)
{
    conVar->Cast<Vector>()->SetValue(value);
}

/**
 * @brief Sets the value of a 4D vector console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 */
extern "C" PLUGIN_API void SetConVarVector4(CConVarBaseData* conVar, const Vector4D& value)
{
    conVar->Cast<Vector4D>()->SetValue(value);
}

/**
 * @brief Sets the value of a quaternion angle console variable.
 *
 * @param conVar Pointer to the console variable data.
 * @param value The value to set for the console variable.
 */
extern "C" PLUGIN_API void SetConVarQangle(CConVarBaseData* conVar, const QAngle& value)
{
    conVar->Cast<QAngle>()->SetValue(value);
}

/**
 * @brief Replicates a console variable value to a specific client. This does not change the actual console variable value.
 *
 * @param conVar Pointer to the console variable data.
 * @param clientIndex The index of the client to replicate the value to.
 * @param value The value to send to the client.
 */
extern "C" PLUGIN_API void SendConVarValue(CConVarBaseData* conVar, int clientIndex, const plg::string& value)
{
	utils::SendConVarValue(CPlayerSlot(clientIndex - 1), conVar, value.c_str());
}

/**
 * @brief Retrieves the value of a client's console variable and stores it in the output string.
 *
 * @param clientIndex The index of the client whose console variable value is being retrieved.
 * @param convarName The name of the console variable to retrieve.
 * @return The output string to store the client's console variable value.
 */
extern "C" PLUGIN_API void GetClientConVarValue(plg::string& output, int clientIndex, const plg::string& convarName)
{
	std::construct_at(&output, g_pEngineServer2->GetClientConVarValue(CPlayerSlot(clientIndex - 1), convarName.c_str()));
}

/**
 * @brief Replicates a console variable value to a specific fake client. This does not change the actual console variable value.
 *
 * @param clientIndex The index of the fake client to replicate the value to.
 * @param convarName The name of the console variable.
 * @param convarValue The value to set for the console variable.
 */
extern "C" PLUGIN_API void SetFakeClientConVarValue(int clientIndex, const plg::string& convarName, const plg::string& convarValue)
{
	g_pEngineServer2->SetFakeClientConVarValue(CPlayerSlot(clientIndex - 1), convarName.c_str(), convarValue.c_str());
}