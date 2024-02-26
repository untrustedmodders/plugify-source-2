#include "message.hpp"

#include <tier0/strtools.h>

LoggingResponse_t CLoggingMessage::Message(const char* pszContent)
{
	return InternalMessage(This::s_eSeverity, pszContent);
}

LoggingResponse_t CLoggingMessage::Message(const Color& aColor, const char* pszContent)
{
	return InternalMessage(This::s_eSeverity, aColor, pszContent);
}

LoggingResponse_t CLoggingMessage::MessageFormat(const char* pszFormat, ...)
{
	char sBuffer[MAX_LOGGING_MESSAGE_LENGTH];

	va_list aParams;

	va_start(aParams, pszFormat);
	V_vsnprintf((char*)sBuffer, sizeof(sBuffer), pszFormat, aParams);
	va_end(aParams);

	return Message((const char*)sBuffer);
}

LoggingResponse_t CLoggingMessage::MessageFormat(const Color& aColor, const char* pszFormat, ...)
{
	char sBuffer[MAX_LOGGING_MESSAGE_LENGTH];

	va_list aParams;

	va_start(aParams, pszFormat);
	V_vsnprintf((char*)sBuffer, sizeof(sBuffer), pszFormat, aParams);
	va_end(aParams);

	return Message(aColor, (const char*)sBuffer);
}

LoggerScope CLoggingMessage::CreateMessagesScope(const char* pszStartWith, const char* pszEnd)
{
#ifdef DEBUG
	char sDebugWith[32];

	char* pDebugWithResult = (char*)sDebugWith;

	V_strncpy(pDebugWithResult, LOGGER_FORMAT_MESSAGE_STARTWITH, sizeof(sDebugWith));

	size_t nResultSize = V_strlen(pDebugWithResult);

	V_strncpy(&pDebugWithResult[nResultSize], pszStartWith, (int)(sizeof(sDebugWith) - nResultSize));

	return {LOGGER_COLOR_MESSAGE, pDebugWithResult, pszEnd};
#else
	return {LOGGER_COLOR_MESSAGE, pszStartWith, pszEnd};
#endif
}
