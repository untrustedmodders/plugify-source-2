# plugify-source-2
# Copyright (C) 2023-2025 untrustedmodders
# Licensed under the MIT license. See LICENSE file in the project root for details.

if(NOT PLUGIFY_DIR)
	message(FATAL_ERROR "PLUGIFY_DIR is empty")
endif()

set(PLUGIFY_BINARY_DIR "plugify")

set(PLUGIFY_COMPILE_DEFINITIONS
		${PLUGIFY_COMPILE_DEFINITIONS}

		PLUGIFY_FORMAT_SUPPORT=$<BOOL:${COMPILER_SUPPORTS_FORMAT}>
		PLUGIFY_IS_DEBUG=$<STREQUAL:${CMAKE_BUILD_TYPE},Debug>
		PLUGIFY_ARCH_BITS=$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,64,32>
)

set(PLUGIFY_LINK_LIBRARIES
		${PLUGIFY_LINK_LIBRARIES}
)

if(NOT COMPILER_SUPPORTS_FORMAT)
	list(APPEND PLUGIFY_LINK_LIBRARIES fmt::fmt-header-only)
endif()

#
# plugify-configs
#
include(InitConfigs)
list(APPEND PLUGIFY_LINK_LIBRARIES plugify-configs)

#
# plugify-assembly
#
add_subdirectory(${PLUGIFY_DIR} ${SOURCESDK_BINARY_DIR})