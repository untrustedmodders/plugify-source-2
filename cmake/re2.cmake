# plugify-source-2
# Copyright (C) 2023-2025 untrustedmodders
# Licensed under the MIT license. See LICENSE file in the project root for details.

if(NOT WIN32)
	include(FetchContent)

	message(STATUS "Pulling and configuring re2")

	find_package(re2 QUIET)
	if(NOT re2_FOUND)
		# There's no re2 library available on this system, download and
		# build it from GitHub.

		# Abseil is a build requirement for re2.
		set(ABSL_PROPAGATE_CXX_STD ON)
		set(ABSL_USE_SYSTEM_INCLUDES ON)
		set(ABSL_ENABLE_INSTALL ON)
		set(BUILD_TESTING OFF)
		FetchContent_Declare(
				absl
				GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
				GIT_TAG 20240116.1
				OVERRIDE_FIND_PACKAGE
		)
		FetchContent_MakeAvailable(absl)

		# Download and build a known-good version of re2.
		set(RE2_BUILD_TESTING OFF)
		FetchContent_Declare(
				re2
				GIT_REPOSITORY https://github.com/google/re2.git
				GIT_TAG 2024-07-02
		)
		FetchContent_MakeAvailable(re2)
	endif()

	set(RE2_LINK_LIBRARIES
			re2::re2
	)
endif()