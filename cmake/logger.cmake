# cs2-sdk-plugin
# Copyright (C) 2024 untrustedmodders
# Licensed under the MIT license. See LICENSE file in the project root for details.

if(NOT LOGGER_DIR)
	message(FATAL_ERROR "LOGGER_DIR is empty")
endif()

set(LOGGER_BINARY_DIR "s2u-logger")

set(LOGGER_INCLUDE_DIRS
	${LOGGER_INCLUDE_DIRS}

	${LOGGER_DIR}/include
)

add_subdirectory(${LOGGER_DIR} ${LOGGER_BINARY_DIR})
