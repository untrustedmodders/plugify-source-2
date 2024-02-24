#pragma once

#include "ilogging.hpp"
#include "scope.hpp"

#define LOGGER_FORMAT_DETAILED_STARTWITH "DEV: "
#define LOGGER_FORMAT_DETAILED_STARTWITH_PREFIX "[%s] " LOGGER_FORMAT_DETAILED_STARTWITH_ADDITIONAL
#define LOGGER_FORMAT_DETAILED LOGGER_FORMAT_DETAILED_STARTWITH_PREFIX "%s\n"
#define LOGGER_COLOR_DETAILED {255, 255, 255, 191}

class CLoggingDetailed : virtual protected ILogging {
	using This = CLoggingDetailed;

public:
	virtual LoggingResponse_t Detailed(const char *pszContent);
	virtual LoggingResponse_t Detailed(const Color &aColor, const char *pszContent);

	virtual LoggingResponse_t DetailedFormat(const char *pszFormat, ...) FMTFUNCTION(2, 3);
	virtual LoggingResponse_t DetailedFormat(const Color &aColor, const char *pszFormat, ...) FMTFUNCTION(3, 4);

public:
	virtual LoggerScope CreateDetailsScope(const char *pszStartWith = "", const char *pszEnd = "\n");

protected:
	static const LoggingSeverity_t s_eSeverity = LS_DETAILED;
}; // CLoggingDetailed