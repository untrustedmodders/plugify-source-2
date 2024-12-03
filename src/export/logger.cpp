#include <plugin_export.h>
/**
 * @brief Registers a new logging channel with specified properties.
 *
 * This function creates a new logging channel, assigning it a name, flags,
 * verbosity level, and color. The channel can be used to log messages
 * according to the defined properties.
 *
 * @param name The name of the logging channel.
 * @param iFlags Flags associated with the logging channel.
 * @param verbosity The verbosity level for the logging channel.
 * @param color The color for messages logged to this channel.
 * @return The ID of the newly created logging channel.
 */
extern "C" PLUGIN_API int RegisterLoggingChannel(const plg::string& name, int iFlags, int verbosity, int color)
{
	Color spewColor;
	spewColor.SetRawColor(color);
	return LoggingSystem_RegisterLoggingChannel(name.c_str(), nullptr, iFlags, static_cast<LoggingVerbosity_t>(verbosity), spewColor);
}

/**
 * @brief Adds a tag to a specified logging channel.
 *
 * This function allows you to associate a tag with a logging channel, enabling
 * better categorization and filtering of logged messages.
 *
 * @param channelID The ID of the logging channel to which the tag will be added.
 * @param tagName The name of the tag to add to the channel.
 */
extern "C" PLUGIN_API void AddLoggerTagToChannel(int channelID, const plg::string& tagName)
{
	LoggingSystem_AddTagToChannel(channelID, tagName.c_str());
}

/**
 * @brief Checks if a specified tag exists in a logging channel.
 *
 * This function verifies whether a specific tag is associated with a
 * logging channel.
 *
 * @param channelID The ID of the logging channel.
 * @param tag The name of the tag to check for.
 * @return True if the tag exists in the channel, otherwise false.
 */
extern "C" PLUGIN_API bool HasLoggerTag(int channelID, const plg::string& tag)
{
	return LoggingSystem_HasTag(channelID, tag.c_str());
}

/**
 * @brief Checks if a logging channel is enabled based on severity.
 *
 * This function determines if the logging channel is active for a
 * given severity level.
 *
 * @param channelID The ID of the logging channel.
 * @param severity The severity level to check.
 * @return True if the channel is enabled for the specified severity, otherwise false.
 */
extern "C" PLUGIN_API bool IsLoggerChannelEnabledBySeverity(int channelID, int severity)
{
	return LoggingSystem_IsChannelEnabled(channelID, static_cast<LoggingSeverity_t>(severity));
}

/**
 * @brief Checks if a logging channel is enabled based on verbosity.
 *
 * This function determines if the logging channel is active for a
 * specified verbosity level.
 *
 * @param channelID The ID of the logging channel.
 * @param verbosity The verbosity level to check.
 * @return True if the channel is enabled for the specified verbosity, otherwise false.
 */
extern "C" PLUGIN_API bool IsLoggerChannelEnabledByVerbosity(int channelID, int verbosity)
{
	return LoggingSystem_IsChannelEnabled(channelID, static_cast<LoggingSeverity_t>(verbosity));
}

/**
 * @brief Retrieves the verbosity level of a logging channel.
 *
 * This function returns the current verbosity level set for a
 * specific logging channel.
 *
 * @param channelID The ID of the logging channel.
 * @return The verbosity level of the specified logging channel.
 */
extern "C" PLUGIN_API int GetLoggerChannelVerbosity(int channelID)
{
	return LoggingSystem_GetChannelVerbosity(channelID);
}

/**
 * @brief Sets the verbosity level of a logging channel.
 *
 * This function updates the verbosity level for a specific logging channel.
 *
 * @param channelID The ID of the logging channel.
 * @param verbosity The new verbosity level to set.
 */
extern "C" PLUGIN_API void SetLoggerChannelVerbosity(int channelID, int verbosity)
{
	return LoggingSystem_SetChannelVerbosity(channelID, static_cast<LoggingVerbosity_t>(verbosity));
}

/**
 * @brief Sets the verbosity level of a logging channel by name.
 *
 * This function updates the verbosity level for a logging channel identified by its name.
 *
 * @param channelID The ID of the logging channel.
 * @param name The name of the logging channel.
 * @param verbosity The new verbosity level to set.
 */
extern "C" PLUGIN_API void SetLoggerChannelVerbosityByName(int channelID, const plg::string& name, int verbosity)
{
	return LoggingSystem_SetChannelVerbosityByName(name.c_str(), static_cast<LoggingVerbosity_t>(verbosity));
}

/**
 * @brief Sets the verbosity level of a logging channel by tag.
 *
 * This function updates the verbosity level for a logging channel identified by its tag.
 *
 * @param channelID The ID of the logging channel.
 * @param tag The name of the tag.
 * @param verbosity The new verbosity level to set.
 */
extern "C" PLUGIN_API void SetLoggerChannelVerbosityByTag(int channelID, const plg::string& tag, int verbosity)
{
	LoggingSystem_SetChannelVerbosityByTag(tag.c_str(), static_cast<LoggingVerbosity_t>(verbosity));
}

/**
 * @brief Retrieves the color setting of a logging channel.
 *
 * This function returns the color associated with a specific logging channel.
 *
 * @param channelID The ID of the logging channel.
 * @return The color value of the specified logging channel.
 */
extern "C" PLUGIN_API int GetLoggerChannelColor(int channelID)
{
	return LoggingSystem_GetChannelColor(channelID);
}

/**
 * @brief Sets the color setting of a logging channel.
 *
 * This function updates the color for messages logged to a specific logging channel.
 *
 * @param channelID The ID of the logging channel.
 * @param color The new color value to set for the channel.
 */
extern "C" PLUGIN_API void SetLoggerChannelColor(int channelID, int color)
{
	LoggingSystem_SetChannelColor(channelID, color);
}

/**
 * @brief Retrieves the flags of a logging channel.
 *
 * This function returns the flags associated with a specific logging channel.
 *
 * @param channelID The ID of the logging channel.
 * @return The flags of the specified logging channel.
 */
extern "C" PLUGIN_API int GetLoggerChannelFlags(int channelID)
{
	return LoggingSystem_GetChannelFlags(channelID);
}

/**
 * @brief Sets the flags of a logging channel.
 *
 * This function updates the flags for a specific logging channel, allowing
 * customization of the logging behavior.
 *
 * @param channelID The ID of the logging channel.
 * @param eFlags The new flags to set for the channel.
 */
extern "C" PLUGIN_API void SetLoggerChannelFlags(int channelID, int eFlags)
{
	LoggingSystem_SetChannelFlags(channelID, static_cast<LoggingChannelFlags_t>(eFlags));
}

/**
 * @brief Logs a message to a specified channel with a severity level.
 *
 * This function logs a message to a specified logging channel and severity level.
 *
 * @param channelID The ID of the logging channel.
 * @param severity The severity level for the log message.
 * @param message The message to log.
 * @return An integer indicating the result of the logging operation.
 */
extern "C" PLUGIN_API int Log(int channelID, int severity, const plg::string& message)
{
	return LoggingSystem_LogDirect(channelID, static_cast<LoggingSeverity_t>(severity), message.c_str());
}

/**
 * @brief Logs a colored message to a specified channel with a severity level.
 *
 * This function logs a message to a specified logging channel with a
 * specific color and severity level.
 *
 * @param channelID The ID of the logging channel.
 * @param severity The severity level for the log message.
 * @param color The color for the log message.
 * @param message The message to log.
 * @return An integer indicating the result of the logging operation.
 */
extern "C" PLUGIN_API int LogColored(int channelID, int severity, int color, const plg::string& message)
{
	Color spewColor;
	spewColor.SetRawColor(color);
	return LoggingSystem_LogDirect(channelID, static_cast<LoggingSeverity_t>(severity), spewColor, message.c_str());
}

/**
 * @brief Logs a detailed message to a specified channel, including source code info.
 *
 * This function logs a message to a specified logging channel, along with
 * the file name, line number, and function name where the log call occurred.
 *
 * @param channelID The ID of the logging channel.
 * @param severity The severity level for the log message.
 * @param file The file name where the log call occurred.
 * @param line The line number where the log call occurred.
 * @param function The name of the function where the log call occurred.
 * @param message The message to log.
 * @return An integer indicating the result of the logging operation.
 */
extern "C" PLUGIN_API int LogFull(int channelID, int severity, const plg::string& file, int line, const plg::string& function, const plg::string& message)
{
	LeafCodeInfo_t codeInfo{file.c_str(), line, function.c_str()};
	return LoggingSystem_LogDirect(channelID, static_cast<LoggingSeverity_t>(severity), codeInfo, message.c_str());
}

/**
 * @brief Logs a detailed colored message to a specified channel, including source code info.
 *
 * This function logs a message with color to a specified logging channel,
 * including details about the file, line number, and function name where
 * the log call occurred.
 *
 * @param channelID The ID of the logging channel.
 * @param severity The severity level for the log message.
 * @param file The file name where the log call occurred.
 * @param line The line number where the log call occurred.
 * @param function The name of the function where the log call occurred.
 * @param color The color for the log message.
 * @param message The message to log.
 * @return An integer indicating the result of the logging operation.
 */
extern "C" PLUGIN_API int LogFullColored(int channelID, int severity, const plg::string& file, int line, const plg::string& function, int color, const plg::string& message)
{
	LeafCodeInfo_t codeInfo{file.c_str(), line, function.c_str()};
	Color spewColor;
	spewColor.SetRawColor(color);
	return LoggingSystem_LogDirect(channelID, static_cast<LoggingSeverity_t>(severity), codeInfo, spewColor, message.c_str());
}
