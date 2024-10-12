# plugify-source-2
# Copyright (C) 2024 untrustedmodders
# Licensed under the MIT license. See LICENSE file in the project root for details.

include(FetchContent)

message(STATUS "Pulling and configuring DynLibUtils")

FetchContent_Declare(
		dynlibutils
		GIT_REPOSITORY https://github.com/Wend4r/cpp-memory_utils.git
		GIT_TAG 59a55a447a5e6951a2b90d598b952d2fb381d295
)

FetchContent_MakeAvailable(dynlibutils)