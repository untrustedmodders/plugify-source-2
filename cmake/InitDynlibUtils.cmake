# plugify-source-2
# Copyright (C) 2023-2025 untrustedmodders
# Licensed under the MIT license. See LICENSE file in the project root for details.

if(NOT DYNLIBUTILS_DIR)
    message(FATAL_ERROR "DYNLIBUTILS_DIR is empty")
endif()

set(DYNLIBUTILS_BINARY_DIR "cpp-memory_utils")

set(DYNLIBUTILS_INCLUDE_DIRS
        ${DYNLIBUTILS_INCLUDE_DIRS}

        ${DYNLIBUTILS_DIR}/include
)

add_subdirectory(${DYNLIBUTILS_DIR} ${DYNLIBUTILS_BINARY_DIR})