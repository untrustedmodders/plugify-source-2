# plugify-source-2
# Copyright (C) 2023-2025 untrustedmodders
# Licensed under the MIT license. See LICENSE file in the project root for details.

if(NOT PLUGIFY_CONFIGS_DIR)
	message(FATAL_ERROR "PLUGIFY_CONFIGS_DIR is empty")
endif()

set(PLUGIFY_CONFIGS_BINARY_DIR "plugify-configs")

add_subdirectory(${PLUGIFY_CONFIGS_DIR} ${PLUGIFY_CONFIGS_BINARY_DIR})
