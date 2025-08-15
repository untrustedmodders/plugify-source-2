#pragma once

#include <functional>
#include <string>

#include <tier0/dbg.h>
#include <tier0/logging.h>

class Logger final {
public:
	Logger(const char* name, RegisterTagsFunc registerTagsFunc, int flags = 0, LoggingVerbosity_t verbosity = LV_DEFAULT, const Color& defaultColor = UNSPECIFIED_LOGGING_COLOR);
	~Logger() = default;

	void AddTagToChannel(const char* tagName) const;
	bool HasTag(const char* tag) const;

	bool IsChannelEnabled(LoggingSeverity_t severity) const;
	bool IsChannelEnabled(LoggingVerbosity_t verbosity) const;

	LoggingVerbosity_t GetChannelVerbosity() const;
	void SetChannelVerbosity(LoggingVerbosity_t verbosity) const;
	int GetChannelColor() const;
	void SetChannelColor(int color) const;
	LoggingChannelFlags_t GetChannelFlags() const;
	void SetChannelFlags(LoggingChannelFlags_t flags) const;

	static void SetChannelVerbosityByName(const char* name, LoggingVerbosity_t verbosity);
	static void SetChannelVerbosityByTag(const char* tag, LoggingVerbosity_t verbosity);
	static void RegisterTags(LoggingChannelID_t) {}

	LoggingResponse_t Log(LoggingSeverity_t severity, const char* content) const;
	LoggingResponse_t Log(LoggingSeverity_t severity, const Color& color, const char* content) const;
	LoggingResponse_t Log(LoggingSeverity_t severity, const LoggingRareOptions_t& code, const char* content) const;
	LoggingResponse_t Log(LoggingSeverity_t severity, const LoggingRareOptions_t& code, const Color& color, const char* content) const;

	LoggingResponse_t LogFormat(LoggingSeverity_t severity, const char* format, ...) const;
	LoggingResponse_t LogFormat(LoggingSeverity_t severity, const Color& color, const char* format, ...) const;
	LoggingResponse_t LogFormat(LoggingSeverity_t severity, const LoggingRareOptions_t& code, const char* format, ...) const;
	LoggingResponse_t LogFormat(LoggingSeverity_t severity, const LoggingRareOptions_t& code, const Color& color, const char* format, ...) const;

private:
	LoggingChannelID_t m_channelID;
};

#ifndef NDEBUG
#define LS_DEBUG LS_MESSAGE
#else
#define LS_DEBUG LS_MESSAGE
#endif

#define S2_CONCAT(a, b) S2_CONCAT_INNER(a, b)
#define S2_CONCAT_INNER(a, b) a ## b
#define S2_UNIQUE_NAME(base) S2_CONCAT(base, __COUNTER__)
#define S2_LOG(...) [[maybe_unused]] auto S2_UNIQUE_NAME(_) = g_Logger.Log(__VA_ARGS__)
#define S2_LOGF(v, ...) [[maybe_unused]] auto S2_UNIQUE_NAME(_) = g_Logger.Log(v, std::format(__VA_ARGS__).c_str())

extern Logger g_Logger;