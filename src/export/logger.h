#pragma once

extern "C" PLUGIN_API LoggingChannelID_t RegisterLoggingChannel(const std::string& name, int iFlags, LoggingVerbosity_t verbosity, int color)
{
	Color spewColor;
	spewColor.SetRawColor(color);
	return LoggingSystem_RegisterLoggingChannel(name.c_str(), &CLogger::RegisterTags, iFlags, verbosity, spewColor);
}

extern "C" PLUGIN_API void AddLoggerTagToChannel(LoggingChannelID_t channelID, const std::string& tagName)
{
	LoggingSystem_AddTagToChannel(channelID, tagName.c_str());
}

extern "C" PLUGIN_API bool HasLoggerTag(LoggingChannelID_t channelID, const std::string& tag)
{
	return LoggingSystem_HasTag(channelID, tag.c_str());
}

extern "C" PLUGIN_API bool IsLoggerChannelEnabledBySeverity(LoggingChannelID_t channelID, LoggingSeverity_t severity)
{
	return LoggingSystem_IsChannelEnabled(channelID, severity);
}

extern "C" PLUGIN_API bool IsLoggerChannelEnabledByVerbosity(LoggingChannelID_t channelID, LoggingVerbosity_t verbosity)
{
	return LoggingSystem_IsChannelEnabled(channelID, verbosity);
}

extern "C" PLUGIN_API LoggingVerbosity_t GetLoggerChannelVerbosity(LoggingChannelID_t channelID)
{
	return LoggingSystem_GetChannelVerbosity(channelID);
}

extern "C" PLUGIN_API void SetLoggerChannelVerbosity(LoggingChannelID_t channelID, LoggingVerbosity_t verbosity)
{
	return LoggingSystem_SetChannelVerbosity(channelID, verbosity);
}

extern "C" PLUGIN_API void SetLoggerChannelVerbosityByName(LoggingChannelID_t channelID, const std::string& name, LoggingVerbosity_t verbosity)
{
	return LoggingSystem_SetChannelVerbosityByName(name.c_str(), verbosity);
}

extern "C" PLUGIN_API void SetLoggerChannelVerbosityByTag(LoggingChannelID_t channelID, const std::string& tag, LoggingVerbosity_t verbosity)
{
	LoggingSystem_SetChannelVerbosityByTag(tag.c_str(), verbosity);
}

extern "C" PLUGIN_API int GetLoggerChannelColor(LoggingChannelID_t channelID)
{
	return LoggingSystem_GetChannelColor(channelID);
}

extern "C" PLUGIN_API void SetLoggerChannelColor(LoggingChannelID_t channelID, int color)
{
	LoggingSystem_SetChannelColor(channelID, color);
}

extern "C" PLUGIN_API LoggingChannelFlags_t GetLoggerChannelFlags(LoggingChannelID_t channelID)
{
	return LoggingSystem_GetChannelFlags(channelID);
}

extern "C" PLUGIN_API void SetLoggerChannelFlags(LoggingChannelID_t channelID, LoggingChannelFlags_t eFlags)
{
	LoggingSystem_SetChannelFlags(channelID, eFlags);
}

extern "C" PLUGIN_API LoggingResponse_t Log(LoggingChannelID_t channelID, LoggingSeverity_t severity, const std::string& message)
{
	return LoggingSystem_LogDirect(channelID, severity, message.c_str());
}

extern "C" PLUGIN_API LoggingResponse_t LogColored(LoggingChannelID_t channelID, LoggingSeverity_t severity, int color, const std::string& message)
{
	Color spewColor;
	spewColor.SetRawColor(color);
	return LoggingSystem_LogDirect(channelID, severity, spewColor, message.c_str());
}

extern "C" PLUGIN_API LoggingResponse_t LogFull(LoggingChannelID_t channelID, LoggingSeverity_t severity, const std::string& file, int line, const std::string& function, const std::string& message)
{
	LeafCodeInfo_t codeInfo{file.c_str(), line, function.c_str()};
	return LoggingSystem_LogDirect(channelID, severity, codeInfo, message.c_str());
}

extern "C" PLUGIN_API LoggingResponse_t LogFullColored(LoggingChannelID_t channelID, LoggingSeverity_t severity, const std::string& file, int line, const std::string& function, int color, const std::string& message)
{
	LeafCodeInfo_t codeInfo{file.c_str(), line, function.c_str()};
	Color spewColor;
	spewColor.SetRawColor(color);
	return LoggingSystem_LogDirect(channelID, severity, codeInfo, spewColor, message.c_str());
}