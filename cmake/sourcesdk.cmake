# plugify-source-2
# Copyright (C) 2024 untrustedmodders
# Licensed under the MIT license. See LICENSE file in the project root for details.

if(NOT SOURCESDK_DIR)
	message(FATAL_ERROR "SOURCESDK_DIR is empty")
endif()

set(SOURCESDK_BINARY_DIR "sourcesdk")

set(SOURCESDK_LINK_STRIP_CPP_EXPORTS ON CACHE INTERNAL "")
set(SOURCESDK_LINK_STEAMWORKS OFF CACHE INTERNAL "")

add_subdirectory(${SOURCESDK_DIR} ${SOURCESDK_BINARY_DIR})
