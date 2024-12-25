# plugify-source-2
# Copyright (C) 2024 untrustedmodders
# Licensed under the MIT license. See LICENSE file in the project root for details.

set(PLUGIFY_COMPILE_DEFINITIONS
		${PLUGIFY_COMPILE_DEFINITIONS}

		PLUGIFY_FORMAT_SUPPORT=$<BOOL:${COMPILER_SUPPORTS_FORMAT}>
		PLUGIFY_IS_DEBUG=$<STREQUAL:${CMAKE_BUILD_TYPE},Debug>
)

set(PLUGIFY_LINK_LIBRARIES
		${PLUGIFY_LINK_LIBRARIES}
)

if(NOT COMPILER_SUPPORTS_FORMAT)
	list(APPEND PLUGIFY_LINK_LIBRARIES fmt::fmt-header-only)
endif()
