#include "logger.h"

CLogger::CLogger(const char* pszName, RegisterTagsFunc pfnRegisterTagsFunc, int iFlags, LoggingVerbosity_t eVerbosity, const Color& aDefault)
{
	m_nChannelID = LoggingSystem_RegisterLoggingChannel(pszName, pfnRegisterTagsFunc, iFlags, eVerbosity, aDefault);
}

void CLogger::AddTagToChannel(const char* pTagName) const
{
	LoggingSystem_AddTagToChannel(m_nChannelID, pTagName);
}

bool CLogger::HasTag(const char* pTag) const
{
	return LoggingSystem_HasTag(m_nChannelID, pTag);
}

bool CLogger::IsChannelEnabled(LoggingSeverity_t eSeverity) const
{
	return LoggingSystem_IsChannelEnabled(m_nChannelID, eSeverity);
}

bool CLogger::IsChannelEnabled(LoggingVerbosity_t eVerbosity) const
{
	return LoggingSystem_IsChannelEnabled(m_nChannelID, eVerbosity);
}

LoggingVerbosity_t CLogger::GetChannelVerbosity() const
{
	return LoggingSystem_GetChannelVerbosity(m_nChannelID);
}

void CLogger::SetChannelVerbosity(LoggingVerbosity_t eVerbosity) const
{
	return LoggingSystem_SetChannelVerbosity(m_nChannelID, eVerbosity);
}

void CLogger::SetChannelVerbosityByName(const char* pName, LoggingVerbosity_t eVerbosity)
{
	return LoggingSystem_SetChannelVerbosityByName(pName, eVerbosity);
}

void CLogger::SetChannelVerbosityByTag(const char* pTag, LoggingVerbosity_t eVerbosity)
{
	LoggingSystem_SetChannelVerbosityByTag(pTag, eVerbosity);
}

int CLogger::GetChannelColor() const
{
	return LoggingSystem_GetChannelColor(m_nChannelID);
}

void CLogger::SetChannelColor(int color) const
{
	LoggingSystem_SetChannelColor(m_nChannelID, color);
}

LoggingChannelFlags_t CLogger::GetChannelFlags() const
{
	return LoggingSystem_GetChannelFlags(m_nChannelID);
}

void CLogger::SetChannelFlags(LoggingChannelFlags_t flags) const
{
	LoggingSystem_SetChannelFlags(m_nChannelID, flags);
}

LoggingResponse_t CLogger::InternalMessage(LoggingSeverity_t eSeverity, const char* pszContent)
{
	LoggingResponse_t eResponse = LR_ABORT;

	if (IsChannelEnabled(eSeverity))
	{
		eResponse = LoggingSystem_LogDirect(m_nChannelID, eSeverity, pszContent);
	}

	return eResponse;
}

LoggingResponse_t CLogger::InternalMessage(LoggingSeverity_t eSeverity, const Color& aColor, const char* pszContent)
{
	LoggingResponse_t eResponse = LR_ABORT;

	if (IsChannelEnabled(eSeverity))
	{
		eResponse = LoggingSystem_LogDirect(m_nChannelID, eSeverity, aColor, pszContent);
	}

	return eResponse;
}

LoggingResponse_t CLogger::InternalMessage(LoggingSeverity_t eSeverity, const LeafCodeInfo_t& aCode, const char* pszContent)
{
	LoggingResponse_t eResponse = LR_ABORT;

	if (IsChannelEnabled(eSeverity))
	{
		eResponse = LoggingSystem_LogDirect(m_nChannelID, eSeverity, aCode, pszContent);
	}

	return eResponse;
}

LoggingResponse_t CLogger::InternalMessage(LoggingSeverity_t eSeverity, const LeafCodeInfo_t& aCode, const Color& aColor, const char* pszContent)
{
	LoggingResponse_t eResponse = LR_ABORT;

	if (IsChannelEnabled(eSeverity))
	{
		eResponse = LoggingSystem_LogDirect(m_nChannelID, eSeverity, aCode, aColor, pszContent);
	}

	return eResponse;
}

LoggingResponse_t CLogger::InternalMessageFormat(LoggingSeverity_t eSeverity, const char* pszFormat, ...)
{
	LoggingResponse_t eResponse = LR_ABORT;

	if (IsChannelEnabled(eSeverity))
	{
		char sBuffer[1024];

		va_list aParams;

		va_start(aParams, pszFormat);
		V_vsnprintf((char*)sBuffer, sizeof(sBuffer), pszFormat, aParams);
		va_end(aParams);

		eResponse = InternalMessage(eSeverity, (const char*)sBuffer);
	}

	return eResponse;
}

LoggingResponse_t CLogger::InternalMessageFormat(LoggingSeverity_t eSeverity, const Color& aColor, const char* pszFormat, ...)
{
	LoggingResponse_t eResponse = LR_ABORT;

	if (IsChannelEnabled(eSeverity))
	{
		char sBuffer[1024];

		va_list aParams;

		va_start(aParams, pszFormat);
		V_vsnprintf((char*)sBuffer, sizeof(sBuffer), pszFormat, aParams);
		va_end(aParams);

		eResponse = InternalMessage(eSeverity, aColor, (const char*)sBuffer);
	}

	return eResponse;
}

LoggingResponse_t CLogger::InternalMessageFormat(LoggingSeverity_t eSeverity, const LeafCodeInfo_t& aCode, const char* pszFormat, ...)
{
	LoggingResponse_t eResponse = LR_ABORT;

	if (IsChannelEnabled(eSeverity))
	{
		char sBuffer[1024];

		va_list aParams;

		va_start(aParams, pszFormat);
		V_vsnprintf((char*)sBuffer, sizeof(sBuffer), pszFormat, aParams);
		va_end(aParams);

		eResponse = InternalMessage(eSeverity, aCode, (const char*)sBuffer);
	}

	return eResponse;
}

LoggingResponse_t CLogger::InternalMessageFormat(LoggingSeverity_t eSeverity, const LeafCodeInfo_t& aCode, const Color& aColor, const char* pszFormat, ...)
{
	LoggingResponse_t eResponse = LR_ABORT;

	if (IsChannelEnabled(eSeverity))
	{
		char sBuffer[1024];

		va_list aParams;

		va_start(aParams, pszFormat);
		V_vsnprintf((char*)sBuffer, sizeof(sBuffer), pszFormat, aParams);
		va_end(aParams);

		eResponse = InternalMessage(eSeverity, aCode, aColor, (const char*)sBuffer);
	}

	return eResponse;
}

CLogger g_Logger(CS2SDK_LOGGING_PREFIX, &CLogger::RegisterTags);