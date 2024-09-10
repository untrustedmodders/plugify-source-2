#include <convar.h>
#include <core/con_var_manager.h>
#include <core/game_config.h>
#include <core/sdk/utils.h>
#include <core/sdk/virtual.h>
#include <plugin_export.h>

// CreateConVar()

extern "C" PLUGIN_API CConVarBaseData* CreateConVarBool(const plg::string& name, bool defaultValue, const plg::string& description, int flags, bool hasMin, bool min, bool hasMax, bool max)
{
	return g_ConVarManager.CreateConVar<bool>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

extern "C" PLUGIN_API CConVarBaseData* CreateConVarInt16(const plg::string& name, int16_t defaultValue, const plg::string& description, int flags, bool hasMin, int16_t min, bool hasMax, int16_t max)
{
	return g_ConVarManager.CreateConVar<int16_t>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

extern "C" PLUGIN_API CConVarBaseData* CreateConVarUInt16(const plg::string& name, uint16_t defaultValue, const plg::string& description, int flags, bool hasMin, uint16_t min, bool hasMax, uint16_t max)
{
	return g_ConVarManager.CreateConVar<uint16_t>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

extern "C" PLUGIN_API CConVarBaseData* CreateConVarInt32(const plg::string& name, int32_t defaultValue, const plg::string& description, int flags, bool hasMin, int32_t min, bool hasMax, int32_t max)
{
	return g_ConVarManager.CreateConVar<int32_t>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

extern "C" PLUGIN_API CConVarBaseData* CreateConVarUInt32(const plg::string& name, uint32_t defaultValue, const plg::string& description, int flags, bool hasMin, uint32_t min, bool hasMax, uint32_t max)
{
	return g_ConVarManager.CreateConVar<uint32_t>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

extern "C" PLUGIN_API CConVarBaseData* CreateConVarInt64(const plg::string& name, int64_t defaultValue, const plg::string& description, int flags, bool hasMin, int64_t min, bool hasMax, int64_t max)
{
	return g_ConVarManager.CreateConVar<int64_t>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

extern "C" PLUGIN_API CConVarBaseData* CreateConVarUInt64(const plg::string& name, uint64_t defaultValue, const plg::string& description, int flags, bool hasMin, uint64_t min, bool hasMax, uint64_t max)
{
	return g_ConVarManager.CreateConVar<uint64_t>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

extern "C" PLUGIN_API CConVarBaseData* CreateConVarFloat(const plg::string& name, float defaultValue, const plg::string& description, int flags, bool hasMin, float min, bool hasMax, float max)
{
	return g_ConVarManager.CreateConVar<float>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

extern "C" PLUGIN_API CConVarBaseData* CreateConVarDouble(const plg::string& name, double defaultValue, const plg::string& description, int flags, bool hasMin, double min, bool hasMax, double max)
{
	return g_ConVarManager.CreateConVar<double>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

extern "C" PLUGIN_API CConVarBaseData* CreateConVar(const plg::string& name, const plg::string& defaultValue, const plg::string& description, int flags)
{
	return g_ConVarManager.CreateConVar<const char*>(name, description, defaultValue.data(), flags);
}

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

extern "C" PLUGIN_API CConVarBaseData* CreateConVarVector2(const plg::string& name, const Vector2D& defaultValue, const plg::string& description, int flags, bool hasMin, const Vector2D& min, bool hasMax, const Vector2D& max)
{
	return g_ConVarManager.CreateConVar<Vector2D>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

extern "C" PLUGIN_API CConVarBaseData* CreateConVarVector3(const plg::string& name, const Vector& defaultValue, const plg::string& description, int flags, bool hasMin, const Vector& min, bool hasMax, const Vector& max)
{
	return g_ConVarManager.CreateConVar<Vector>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

extern "C" PLUGIN_API CConVarBaseData* CreateConVarVector4(const plg::string& name, const Vector4D& defaultValue, const plg::string& description, int flags, bool hasMin, const Vector4D& min, bool hasMax, const Vector4D& max)
{
	return g_ConVarManager.CreateConVar<Vector4D>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

extern "C" PLUGIN_API CConVarBaseData* CreateConVarQangle(const plg::string& name, const QAngle& defaultValue, const plg::string& description, int flags, bool hasMin, const QAngle& min, bool hasMax, const QAngle& max)
{
	return g_ConVarManager.CreateConVar<QAngle>(name, description, defaultValue, flags, hasMin, min, hasMax, max);
}

extern "C" PLUGIN_API CConVarBaseData* FindConVar(const plg::string& name)
{
	return g_ConVarManager.FindConVar(name);
}

extern "C" PLUGIN_API void HookConVarChange(const plg::string& name, ConVarChangeListenerCallback callback)
{
	g_ConVarManager.HookConVarChange(name, callback);
}

extern "C" PLUGIN_API void UnhookConVarChange(const plg::string& name, ConVarChangeListenerCallback callback)
{
	g_ConVarManager.UnhookConVarChange(name, callback);
}

extern "C" PLUGIN_API bool IsConVarFlagSet(CConVarBaseData* conVar, int64_t flag)
{
	return conVar->IsFlagSet(flag);
}

extern "C" PLUGIN_API void AddConVarFlags(CConVarBaseData* conVar, int64_t flags)
{
	conVar->AddFlags(flags);
}

extern "C" PLUGIN_API void RemoveConVarFlags(CConVarBaseData* conVar, int64_t flags)
{
	conVar->RemoveFlags(flags);
}

extern "C" PLUGIN_API int64_t GetConVarFlags(CConVarBaseData* conVar)
{
	return conVar->GetFlags();
}

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
			std::stringstream ss;
			ss << value.r() << " " <<  value.g() << " " << value.b() << " " << value.a();
			std::construct_at(&output, ss.str());
			break;
		}
		case EConVarType_Vector2:
		{
			const auto& value = conVar->Cast<Vector2D>()->GetValue();
			std::stringstream ss;
			ss << value.x << " " <<  value.y;
			std::construct_at(&output, ss.str());
			break;
		}
		case EConVarType_Vector3:
		{
			const auto& value = conVar->Cast<Vector>()->GetValue();
			std::stringstream ss;
			ss << value.x << " " <<  value.y << " " << value.z;
			std::construct_at(&output, ss.str());
			break;
		}
		case EConVarType_Vector4:
		{
			const auto& value = conVar->Cast<Vector4D>()->GetValue();
			std::stringstream ss;
			ss << value.x << " " <<  value.y << " " << value.z << " " << value.w;
			std::construct_at(&output, ss.str());
			break;
		}
		case EConVarType_Qangle:
		{
			const auto& value = conVar->Cast<QAngle>()->GetValue();
			std::stringstream ss;
			ss << value.x << " " <<  value.y << " " << value.z;
			std::construct_at(&output, ss.str());
			break;
		}
		default:
			g_Logger.Log(LS_WARNING, "Invalid convar type.\n");
			break;
	}
}

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

extern "C" PLUGIN_API void SendConVarValue(CConVarBaseData* conVar, int clientIndex, const plg::string& value)
{
	utils::SendConVarValue(CPlayerSlot(clientIndex - 1), conVar, value.c_str());
}

extern "C" PLUGIN_API void GetClientConVarValue(plg::string& output, int clientIndex, const plg::string& convarName)
{
	std::construct_at(&output, g_pEngineServer2->GetClientConVarValue(CPlayerSlot(clientIndex - 1), convarName.c_str()));
}

extern "C" PLUGIN_API void SetFakeClientConVarValue(int clientIndex, const plg::string& convarName, const plg::string& convarValue)
{
	g_pEngineServer2->SetFakeClientConVarValue(CPlayerSlot(clientIndex - 1), convarName.c_str(), convarValue.c_str());
}