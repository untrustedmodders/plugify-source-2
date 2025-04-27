# plugify-source-2
# Copyright (C) 2023-2025 untrustedmodders
# Licensed under the MIT license. See LICENSE file in the project root for details.

include(FetchContent)

message(STATUS "Pulling and configuring DynLibUtils")

FetchContent_Declare(
		dynlibutils
		GIT_REPOSITORY https://github.com/Wend4r/cpp-memory_utils.git
		GIT_TAG 1.0.0
		GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(dynlibutils)