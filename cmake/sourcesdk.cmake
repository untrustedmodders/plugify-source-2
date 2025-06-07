# plugify-source-2
# Copyright (C) 2023-2025 untrustedmodders
# Licensed under the MIT license. See LICENSE file in the project root for details.

if(NOT SOURCESDK_DIR)
	message(FATAL_ERROR "SOURCESDK_DIR is empty")
endif()

set(SOURCESDK_BINARY_DIR "sourcesdk")

set(SOURCESDK_LINK_STRIP_CPP_EXPORTS ON CACHE INTERNAL "")
set(SOURCESDK_LINK_STEAMWORKS ON CACHE INTERNAL "")
set(SOURCESDK_CONFIGURE_EXPORT_MAP OFF CACHE INTERNAL "")

if(SOURCESDK_GAME_TARGET STREQUAL "deadlock")
	set(SOURCESDK_PROTOS gcsdk_gcmessages networkbasetypes network_connection CACHE INTERNAL "")
endif()

add_subdirectory(${SOURCESDK_DIR} ${SOURCESDK_BINARY_DIR})
