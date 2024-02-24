#pragma once

extern "C" PLUGIN_API LoggingChannelID_t Logger_RegisterLoggingChannel(const std::string& name, int iFlags, LoggingVerbosity_t verbosity, int color)
{
	Color spewColor;
	spewColor.SetRawColor(color);
	return LoggingSystem_RegisterLoggingChannel(name.c_str(), &CLogger::RegisterTags, iFlags, verbosity, spewColor);
}

extern "C" PLUGIN_API void Logger_AddTagToChannel(LoggingChannelID_t channelID, const std::string& tagName)
{
	LoggingSystem_AddTagToChannel(channelID, tagName.c_str());
}

extern "C" PLUGIN_API bool Logger_HasTag(LoggingChannelID_t channelID, const std::string& tag)
{
	return LoggingSystem_HasTag(channelID, tag.c_str());
}

extern "C" PLUGIN_API bool Logger_IsChannelEnabledBySeverity(LoggingChannelID_t channelID, LoggingSeverity_t severity)
{
	return LoggingSystem_IsChannelEnabled(channelID, severity);
}

extern "C" PLUGIN_API bool Logger_IsChannelEnabledByVerbosity(LoggingChannelID_t channelID, LoggingVerbosity_t verbosity)
{
	return LoggingSystem_IsChannelEnabled(channelID, verbosity);
}

extern "C" PLUGIN_API LoggingVerbosity_t Logger_GetChannelVerbosity(LoggingChannelID_t channelID)
{
	return LoggingSystem_GetChannelVerbosity(channelID);
}

extern "C" PLUGIN_API void Logger_SetChannelVerbosity(LoggingChannelID_t channelID, LoggingVerbosity_t verbosity)
{
	return LoggingSystem_SetChannelVerbosity(channelID, verbosity);
}

extern "C" PLUGIN_API void Logger_SetChannelVerbosityByName(LoggingChannelID_t channelID, const std::string& name, LoggingVerbosity_t verbosity)
{
	return LoggingSystem_SetChannelVerbosityByName(name.c_str(), verbosity);
}

extern "C" PLUGIN_API void Logger_SetChannelVerbosityByTag(LoggingChannelID_t channelID, const std::string& tag, LoggingVerbosity_t verbosity)
{
	LoggingSystem_SetChannelVerbosityByTag(tag.c_str(), verbosity);
}

extern "C" PLUGIN_API int Logger_GetChannelColor(LoggingChannelID_t channelID)
{
	return LoggingSystem_GetChannelColor(channelID);
}

extern "C" PLUGIN_API void Logger_SetChannelColor(LoggingChannelID_t channelID, int color)
{
	LoggingSystem_SetChannelColor(channelID, color);
}

extern "C" PLUGIN_API LoggingChannelFlags_t Logger_GetChannelFlags(LoggingChannelID_t channelID)
{
	return LoggingSystem_GetChannelFlags(channelID);
}

extern "C" PLUGIN_API void Logger_SetChannelFlags(LoggingChannelID_t channelID, LoggingChannelFlags_t eFlags)
{
	LoggingSystem_SetChannelFlags(channelID, eFlags);
}

extern "C" PLUGIN_API LoggingResponse_t Logger_LogDirect(LoggingChannelID_t channelID, LoggingSeverity_t severity, const std::string& message)
{
	return LoggingSystem_LogDirect(channelID, severity, message.c_str());
}

extern "C" PLUGIN_API LoggingResponse_t Logger_LogDirectColored(LoggingChannelID_t channelID, LoggingSeverity_t severity, int color, const std::string& message)
{
	Color spewColor;
	spewColor.SetRawColor(color);
	return LoggingSystem_LogDirect(channelID, severity, spewColor, message.c_str());
}

extern "C" PLUGIN_API LoggingResponse_t Logger_LogDirectFull(LoggingChannelID_t channelID, LoggingSeverity_t severity, const std::string& file, int line, const std::string& function, const std::string& message)
{
	LeafCodeInfo_t codeInfo{file.c_str(), line, function.c_str()};
	return LoggingSystem_LogDirect(channelID, severity, codeInfo, message.c_str());
}

extern "C" PLUGIN_API LoggingResponse_t Logger_LogDirectFullColored(LoggingChannelID_t channelID, LoggingSeverity_t severity, const std::string& file, int line, const std::string& function, int color, const std::string& message)
{
	LeafCodeInfo_t codeInfo{file.c_str(), line, function.c_str()};
	Color spewColor;
	spewColor.SetRawColor(color);
	return LoggingSystem_LogDirect(channelID, severity, codeInfo, spewColor, message.c_str());
}