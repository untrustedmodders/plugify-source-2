#pragma once

#include <functional>
#include <string>

#include <tier0/dbg.h>
#include <tier0/logging.h>

#include "logger/detailed.hpp"
#include "logger/error.hpp"
#include "logger/message.hpp"
#include "logger/throw_assert.hpp"
#include "logger/warning.hpp"

class CLogger final : public CLoggingDetailed, public CLoggingMessage, public CLoggingWarning, public CLoggingThrowAssert, public CLoggingError
{
public:
	CLogger(const char* pszName, RegisterTagsFunc pfnRegisterTagsFunc, int iFlags = 0, LoggingVerbosity_t eVerbosity = LV_DEFAULT, const Color& aDefault = UNSPECIFIED_LOGGING_COLOR);
	~CLogger() = default;

	void AddTagToChannel(const char* pTagName) const;
	bool HasTag(const char* pTag) const;

	bool IsChannelEnabled(LoggingSeverity_t eSeverity) const;
	bool IsChannelEnabled(LoggingVerbosity_t eVerbosity) const;

	LoggingVerbosity_t GetChannelVerbosity() const;
	void SetChannelVerbosity(LoggingVerbosity_t eVerbosity) const;
	int GetChannelColor() const;
	void SetChannelColor(int color) const;
	LoggingChannelFlags_t GetChannelFlags() const;
	void SetChannelFlags(LoggingChannelFlags_t flags) const;

	static void SetChannelVerbosityByName(const char* pName, LoggingVerbosity_t eVerbosity);
	static void SetChannelVerbosityByTag(const char* pTag, LoggingVerbosity_t eVerbosity);
	static void RegisterTags(LoggingChannelID_t channelID) {}

protected:
	LoggingResponse_t InternalMessage(LoggingSeverity_t eSeverity, const char* pszContent) override;
	LoggingResponse_t InternalMessage(LoggingSeverity_t eSeverity, const Color& aColor, const char* pszContent) override;
	LoggingResponse_t InternalMessage(LoggingSeverity_t eSeverity, const LeafCodeInfo_t& aCode, const char* pszContent) override;
	LoggingResponse_t InternalMessage(LoggingSeverity_t eSeverity, const LeafCodeInfo_t& aCode, const Color& aColor, const char* pszContent) override;

	LoggingResponse_t InternalMessageFormat(LoggingSeverity_t eSeverity, const char* pszFormat, ...) override;
	LoggingResponse_t InternalMessageFormat(LoggingSeverity_t eSeverity, const Color& aColor, const char* pszFormat, ...) override;
	LoggingResponse_t InternalMessageFormat(LoggingSeverity_t eSeverity, const LeafCodeInfo_t& aCode, const char* pszFormat, ...) override;
	LoggingResponse_t InternalMessageFormat(LoggingSeverity_t eSeverity, const LeafCodeInfo_t& aCode, const Color& aColor, const char* pszFormat, ...) override;

private:
	LoggingChannelID_t m_nChannelID;
};

extern CLogger g_Logger;