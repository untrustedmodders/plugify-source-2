# cs2-sdk-plugin
# Copyright (C) 2024 untrustedmodders
# Licensed under the MIT license. See LICENSE file in the project root for details.

if(UNIX)
	if(APPLE)
		set(MACOS TRUE)
	else()
		set(LINUX TRUE)
	endif()
endif()

if(WIN32)
	if(NOT MSVC)
		message(FATAL_ERROR "MSVC restricted")
	endif()

	set(WINDOWS TRUE)
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING
	"Only do Release and Debug"
	FORCE
)
