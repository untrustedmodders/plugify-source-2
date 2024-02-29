#pragma once

extern "C" PLUGIN_API int RegisterLoggingChannel(const std::string& name, int iFlags, int verbosity, int color)
{
	Color spewColor;
	spewColor.SetRawColor(color);
	return LoggingSystem_RegisterLoggingChannel(name.c_str(), &CLogger::RegisterTags, iFlags, static_cast<LoggingVerbosity_t>(verbosity), spewColor);
}

extern "C" PLUGIN_API void AddLoggerTagToChannel(int channelID, const std::string& tagName)
{
	LoggingSystem_AddTagToChannel(channelID, tagName.c_str());
}

extern "C" PLUGIN_API bool HasLoggerTag(int channelID, const std::string& tag)
{
	return LoggingSystem_HasTag(channelID, tag.c_str());
}

extern "C" PLUGIN_API bool IsLoggerChannelEnabledBySeverity(int channelID, int severity)
{
	return LoggingSystem_IsChannelEnabled(channelID, static_cast<LoggingSeverity_t>(severity));
}

extern "C" PLUGIN_API bool IsLoggerChannelEnabledByVerbosity(int channelID, int verbosity)
{
	return LoggingSystem_IsChannelEnabled(channelID, static_cast<LoggingSeverity_t>(verbosity));
}

extern "C" PLUGIN_API int GetLoggerChannelVerbosity(int channelID)
{
	return LoggingSystem_GetChannelVerbosity(channelID);
}

extern "C" PLUGIN_API void SetLoggerChannelVerbosity(int channelID, int verbosity)
{
	return LoggingSystem_SetChannelVerbosity(channelID, static_cast<LoggingVerbosity_t>(verbosity));
}

extern "C" PLUGIN_API void SetLoggerChannelVerbosityByName(int channelID, const std::string& name, int verbosity)
{
	return LoggingSystem_SetChannelVerbosityByName(name.c_str(), static_cast<LoggingVerbosity_t>(verbosity));
}

extern "C" PLUGIN_API void SetLoggerChannelVerbosityByTag(int channelID, const std::string& tag, int verbosity)
{
	LoggingSystem_SetChannelVerbosityByTag(tag.c_str(), static_cast<LoggingVerbosity_t>(verbosity));
}

extern "C" PLUGIN_API int GetLoggerChannelColor(int channelID)
{
	return LoggingSystem_GetChannelColor(channelID);
}

extern "C" PLUGIN_API void SetLoggerChannelColor(int channelID, int color)
{
	LoggingSystem_SetChannelColor(channelID, color);
}

extern "C" PLUGIN_API int GetLoggerChannelFlags(int channelID)
{
	return LoggingSystem_GetChannelFlags(channelID);
}

extern "C" PLUGIN_API void SetLoggerChannelFlags(int channelID, int eFlags)
{
	LoggingSystem_SetChannelFlags(channelID, static_cast<LoggingChannelFlags_t>(eFlags));
}

extern "C" PLUGIN_API int Log(int channelID, int severity, const std::string& message)
{
	return LoggingSystem_LogDirect(channelID, static_cast<LoggingSeverity_t>(severity), message.c_str());
}

extern "C" PLUGIN_API int LogColored(int channelID, int severity, int color, const std::string& message)
{
	Color spewColor;
	spewColor.SetRawColor(color);
	return LoggingSystem_LogDirect(channelID, static_cast<LoggingSeverity_t>(severity), spewColor, message.c_str());
}

extern "C" PLUGIN_API int LogFull(int channelID, int severity, const std::string& file, int line, const std::string& function, const std::string& message)
{
	LeafCodeInfo_t codeInfo{file.c_str(), line, function.c_str()};
	return LoggingSystem_LogDirect(channelID, static_cast<LoggingSeverity_t>(severity), codeInfo, message.c_str());
}

extern "C" PLUGIN_API int LogFullColored(int channelID, int severity, const std::string& file, int line, const std::string& function, int color, const std::string& message)
{
	LeafCodeInfo_t codeInfo{file.c_str(), line, function.c_str()};
	Color spewColor;
	spewColor.SetRawColor(color);
	return LoggingSystem_LogDirect(channelID, static_cast<LoggingSeverity_t>(severity), codeInfo, spewColor, message.c_str());
}