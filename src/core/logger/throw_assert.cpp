#include "throw_assert.hpp"

#include <tier0/strtools.h>

LoggingResponse_t CLoggingThrowAssert::ThrowAssert(const LeafCodeInfo_t& aCode, const char* pszContent)
{
	return InternalMessage(This::s_eSeverity, aCode, pszContent);
}

LoggingResponse_t CLoggingThrowAssert::ThrowAssert(const LeafCodeInfo_t& aCode, const Color& aColor, const char* pszContent)
{
	return InternalMessage(This::s_eSeverity, aCode, aColor, pszContent);
}

LoggingResponse_t CLoggingThrowAssert::ThrowAssertFormat(const LeafCodeInfo_t& aCode, const char* pszFormat, ...)
{
	char sBuffer[MAX_LOGGING_MESSAGE_LENGTH];

	va_list aParams;

	va_start(aParams, pszFormat);
	V_vsnprintf((char*)sBuffer, sizeof(sBuffer), pszFormat, aParams);
	va_end(aParams);

	return ThrowAssert(aCode, (const char*)sBuffer);
}

LoggingResponse_t CLoggingThrowAssert::ThrowAssertFormat(const LeafCodeInfo_t& aCode, const Color& aColor, const char* pszFormat, ...)
{
	char sBuffer[MAX_LOGGING_MESSAGE_LENGTH];

	va_list aParams;

	va_start(aParams, pszFormat);
	V_vsnprintf((char*)sBuffer, sizeof(sBuffer), pszFormat, aParams);
	va_end(aParams);

	return ThrowAssert(aCode, aColor, (const char*)sBuffer);
}

LoggerScope CLoggingThrowAssert::CreateAssertScope(const char* pszStartWith, const char* pszEnd)
{
#ifdef DEBUG
	char sDebugWith[32];

	char* pDebugWithResult = (char*)sDebugWith;

	V_strncpy(pDebugWithResult, LOGGER_FORMAT_ASSERT_STARTWITH, sizeof(sDebugWith));

	size_t nResultSize = V_strlen(pDebugWithResult);

	V_strncpy(&pDebugWithResult[nResultSize], pszStartWith, (int)(sizeof(sDebugWith) - nResultSize));

	return {LOGGER_COLOR_ASSERT, pDebugWithResult, pszEnd};
#else
	return {LOGGER_COLOR_ASSERT, pszStartWith, pszEnd};
#endif
}
