# plugify-source2
# Copyright (C) 2023-2025 untrustedmodders
# Licensed under the GPLv3 license. See LICENSE file in the project root for details.

cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

if(POLICY CMP0092)
    cmake_policy(SET CMP0092 NEW) # Don't add -W3 warning level by default.
endif()


file(READ "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" VERSION_FILE_CONTENTS)
string(STRIP "${VERSION_FILE_CONTENTS}" VERSION_FILE_CONTENTS)
set(S2SDK_VERSION "${VERSION_FILE_CONTENTS}" CACHE STRING "Set version name")
set(S2SDK_PACKAGE "cs2sdk" CACHE STRING "Set package name")
string(REPLACE "v" "" S2SDK_VERSION "${S2SDK_VERSION}")
string(REGEX REPLACE "[.+-]" ";" S2SDK_VERSION_LIST ${S2SDK_VERSION})
list(GET S2SDK_VERSION_LIST 0 S2SDK_VERSION_MAJOR)
list(GET S2SDK_VERSION_LIST 1 S2SDK_VERSION_MINOR)
list(GET S2SDK_VERSION_LIST 2 S2SDK_VERSION_PATCH)

project(plugify-source-2
		VERSION "${S2SDK_VERSION_MAJOR}.${S2SDK_VERSION_MINOR}.${S2SDK_VERSION_PATCH}"
		DESCRIPTION "Source2 SDK Plugin"
		HOMEPAGE_URL "https://github.com/untrustedmodders/plugify-source-2"
		LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE MATCHES "Debug|Devel|MinSizeRel|RelWithDebInfo|Release")
    message(STATUS "CMAKE_BUILD_TYPE not set, defaulting to Debug.")
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(UNIX)
    if(APPLE)
        set(MACOS TRUE)
    else()
        set(LINUX TRUE)
    endif()
endif()

if(WIN32)
    if(NOT MSVC)
        message(FATAL_ERROR "MSVC restricted")
    endif()

    set(WINDOWS TRUE)
endif()

string(REGEX REPLACE "^[^-]*-" "" PROJECT_NAME_SUBSTRING "${PROJECT_NAME}")
string(TOLOWER "${PROJECT_NAME_SUBSTRING}" PROJECT_NAME_LOWER)
string(TOUPPER "${PROJECT_NAME_SUBSTRING}" PROJECT_NAME_UPPER)

#
# Plugufy
#
set(PLUGIFY_BUILD_SHARED_LIB ON CACHE INTERNAL "")
set(PLUGIFY_BUILD_JIT OFF CACHE INTERNAL "")
set(PLUGIFY_BUILD_ASSEMBLY ON CACHE INTERNAL "")
set(PLUGIFY_BUILD_TESTS OFF CACHE INTERNAL "")
set(PLUGIFY_INTERFACE ON CACHE INTERNAL "")
set(PLUGIFY_DOWNLOADER OFF CACHE INTERNAL "")
if(LINUX)
    set(PLUGIFY_USE_STATIC_STDLIB ON CACHE INTERNAL "")
endif()
add_subdirectory(external/plugify)
add_subdirectory(external/plugify-configs)

#
# SourceSDK
#
set(SOURCESDK_LINK_STRIP_CPP_EXPORTS ON CACHE INTERNAL "")
set(SOURCESDK_LINK_STEAMWORKS ON CACHE INTERNAL "")
set(SOURCESDK_CONFIGURE_EXPORT_MAP OFF CACHE INTERNAL "")

if(SOURCESDK_GAME_TARGET STREQUAL "deadlock")
    set(SOURCESDK_PROTOS gcsdk_gcmessages networkbasetypes network_connection CACHE INTERNAL "")
endif()

add_subdirectory(external/sourcesdk)

set(PROJECT_OUTPUT_NAME "s2sdk")
set(S2SDK_COMPILE_DEFINITIONS
        S2SDK_LOGGING_PREFIX="${PROJECT_OUTPUT_NAME}"
        S2SDK_GAME_NAME="${S2SDK_GAME_NAME}"
        S2SDK_PLATFORM_WINDOWS=$<BOOL:${WINDOWS}>
        S2SDK_PLATFORM_APPLE=$<BOOL:${APPLE}>
        S2SDK_PLATFORM_LINUX=$<BOOL:${LINUX}>
        S2SDK_LIBRARY_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}"
        S2SDK_LIBRARY_PREFIX="${CMAKE_SHARED_LIBRARY_PREFIX}"

        PLUGIFY_FORMAT_SUPPORT=$<BOOL:${COMPILER_SUPPORTS_FORMAT}>
        PLUGIFY_IS_DEBUG=$<STREQUAL:${CMAKE_BUILD_TYPE},Debug>
        PLUGIFY_ARCH_BITS=$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,64,32>
)

if(WINDOWS)
    set(S2SDK_COMPILE_DEFINITIONS
			${S2SDK_COMPILE_DEFINITIONS}
            S2SDK_PLATFORM="win64"
            S2SDK_ROOT_BINARY="/bin/win64/"
            S2SDK_GAME_BINARY="/csgo/bin/win64/"
    )
elseif(LINUX)
    set(S2SDK_COMPILE_DEFINITIONS
            ${S2SDK_COMPILE_DEFINITIONS}
            S2SDK_PLATFORM="linuxsteamrt64"
            S2SDK_ROOT_BINARY="/bin/linuxsteamrt64/"
            S2SDK_GAME_BINARY="/bin/linuxsteamrt64/../../csgo/bin/linuxsteamrt64/" # Metamod hack
    )
endif()

file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_precompile_headers(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.hpp")

set_target_properties(${PROJECT_NAME} PROPERTIES
		C_STANDARD 11
		C_STANDARD_REQUIRED ON
		C_EXTENSIONS OFF

		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_OUTPUT_NAME})

if(WINDOWS)
    set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_target_properties(plugify-assembly PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(MACOS)
    set_target_properties(${PROJECT_NAME} PROPERTIES OSX_ARCHITECTURES "x86_64")
endif()

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME} EXPORT_MACRO_NAME PLUGIN_API EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/plugin_export.h)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/exports)

target_link_libraries(${PROJECT_NAME} PRIVATE sourcesdk plugify-configs plugify-assembly)

if(MACOS)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-Wl,-exported_symbols_list,${CMAKE_CURRENT_SOURCE_DIR}/sym/exported_symbols.lds")
elseif(LINUX)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-Wl,--version-script,${CMAKE_CURRENT_SOURCE_DIR}/sym/version_script.lds")
endif()

if(LINUX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _GLIBCXX_USE_CXX11_ABI=0)
    target_link_libraries(${PROJECT_NAME} PRIVATE -static-libstdc++ -static-libgcc)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE ${S2SDK_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(NOT COMPILER_SUPPORTS_FORMAT)
    target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt-header-only)
endif()

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/s2sdk.pplugin.in
        ${CMAKE_CURRENT_BINARY_DIR}/s2sdk.pplugin
)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/gamedata.jsonc
        ${CMAKE_CURRENT_BINARY_DIR}/gamedata.jsonc
        COPYONLY
)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/settings.jsonc
        ${CMAKE_CURRENT_BINARY_DIR}/settings.jsonc
        COPYONLY
)

