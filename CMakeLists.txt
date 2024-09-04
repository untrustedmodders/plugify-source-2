cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

if(POLICY CMP0092)
    cmake_policy(SET CMP0092 NEW) # Don't add -W3 warning level by default.
endif()
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()


project(cs2-sdk-plugin VERSION 1.0.0.0 DESCRIPTION "CS2 SDK Plugin" HOMEPAGE_URL "https://github.com/untrustedmodders/cs2-sdk-plugin" LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(WIN32 AND NOT MSVC)
    message(FATAL "MSVC restricted.")
endif()

add_subdirectory(external/sourcesdk)

# Sources
file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/external/dynlibutils/module.cpp)

set(SOURCESDK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/sourcesdk)
set(METAMOD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/metamod-source)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${SOURCESDK_DIR}
        ${SOURCESDK_DIR}/thirdparty/protobuf/src
        ${SOURCESDK_DIR}/common
        ${SOURCESDK_DIR}/game/shared
        ${SOURCESDK_DIR}/game/server
        ${SOURCESDK_DIR}/public
        ${SOURCESDK_DIR}/public/engine
        ${SOURCESDK_DIR}/public/mathlib
        ${SOURCESDK_DIR}/public/tier0
        ${SOURCESDK_DIR}/public/tier1
        ${SOURCESDK_DIR}/public/entity2
        ${SOURCESDK_DIR}/public/game/server
        ${SOURCESDK_DIR}/public/entity2

        ${METAMOD_DIR}/core
        ${METAMOD_DIR}/core/sourcehook

        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/cs2_sdk
        ${CMAKE_CURRENT_SOURCE_DIR}/external/dynlibutils
)

target_precompile_headers(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME} EXPORT_MACRO_NAME PLUGIN_API EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/plugin_export.h)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/exports)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "cs2sdk")

target_compile_definitions(${PROJECT_NAME} PRIVATE
        CS2SDK_LOGGING_PREFIX="cs2-sdk"
        CS2SDK_PLATFORM_WINDOWS=$<BOOL:${WIN32}>
        CS2SDK_PLATFORM_APPLE=$<BOOL:${APPLE}>
        CS2SDK_PLATFORM_LINUX=$<BOOL:${LINUX}>
        CS2SDK_LIBRARY_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}"
        CS2SDK_LIBRARY_PREFIX="${CMAKE_SHARED_LIBRARY_PREFIX}"
        CS2SDK_PLATFORM="linux"
        CS2SDK_BINARY="linuxsteamrt64"
        CS2SDK_ROOT_BINARY="/bin/linuxsteamrt64/"
        # Metamod hack
        CS2SDK_GAME_BINARY="/bin/linuxsteamrt64/../../csgo/bin/linuxsteamrt64/"
)

if(LINUX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            META_IS_SOURCE2
            _LINUX
            POSIX
            LINUX
            GNUC
            COMPILER_GCC
            PLATFORM_64BITS
            bswap_16=__bswap_16
            bswap_32=__bswap_32
            bswap_64=__bswap_64
            _GLIBCXX_USE_CXX11_ABI=0
    )
elseif(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            META_IS_SOURCE2
            COMPILER_MSVC
            COMPILER_MSVC64
            _WIN32
            _WINDOWS
            _ALLOW_KEYWORD_MACROS
            __STDC_LIMIT_MACROS
            _CRT_SECURE_NO_WARNINGS=1
            _CRT_SECURE_NO_DEPRECATE=1
            _CRT_NONSTDC_NO_DEPRECATE=1
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()


# Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE sourcesdk)

set(CS2SDK_VERSION "0" CACHE STRING "Set version name")
set(CS2SDK_PACKAGE "cs2sdk" CACHE STRING "Set package name")

configure_file(
        ${CMAKE_SOURCE_DIR}/cs2sdk.pplugin.in
        ${CMAKE_BINARY_DIR}/cs2sdk.pplugin
)