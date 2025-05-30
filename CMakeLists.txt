cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

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

option(S2SDK_BUILD_DOCS "Enable building with documentation." OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

string(REGEX REPLACE "^[^-]*-" "" PROJECT_NAME_SUBSTRING "${PROJECT_NAME}")
string(TOLOWER "${PROJECT_NAME_SUBSTRING}" PROJECT_NAME_LOWER)
string(TOUPPER "${PROJECT_NAME_SUBSTRING}" PROJECT_NAME_UPPER)

set(PROJECT_OUTPUT_NAME "s2sdk")

set(PROJECT_AUTHOR "Untrusted Modders Team")
set(PROJECT_DESCRIPTION_FULL "Source 2 SDK Plugify plugin")
set(PROJECT_LICENSE "GPLv3")
string(TIMESTAMP PROJECT_BUILD_DATE "%Y-%m-%d")
string(TIMESTAMP PROJECT_BUILD_TIME "%H:%M:%S")

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SYMBOLS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/sym")
set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")
set(EXPORTS_BINARY_DIR "${CMAKE_BINARY_DIR}/exports")

function(set_or_external_dir VAR_NAME TARGET)
    if(${VAR_NAME})
        file(TO_CMAKE_PATH "${${VAR_NAME}}" ${VAR_NAME})
    else()
        set(${VAR_NAME} "${EXTERNAL_DIR}/${TARGET}")
    endif()

    set(${VAR_NAME} "${${VAR_NAME}}" PARENT_SCOPE)
endfunction()

set_or_external_dir(DYNLIBUTILS_DIR "dynlibutils")
set_or_external_dir(METAMOD_DIR "metamod-source")
set_or_external_dir(PLUGIFY_CONFIGS_DIR "plugify-configs")
set_or_external_dir(PLUGIFY_DIR "plugify")
set_or_external_dir(SOURCESDK_DIR "sourcesdk")

include(GenerateExportHeader)

if(NOT S2SDK_BUILD_DOCS)
    include(cmake/platform/shared.cmake)
    include(cmake/compat_format.cmake)
    include(cmake/dynlibutils.cmake)
    include(cmake/metamod.cmake)
    include(cmake/plugify.cmake)
    include(cmake/sourcesdk.cmake)
endif()

set(COMPILE_DEFINITIONS
        S2SDK_LOGGING_PREFIX="s2sdk"
        S2SDK_PLATFORM_WINDOWS=$<BOOL:${WINDOWS}>
        S2SDK_PLATFORM_APPLE=$<BOOL:${APPLE}>
        S2SDK_PLATFORM_LINUX=$<BOOL:${LINUX}>
        S2SDK_LIBRARY_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}"
        S2SDK_LIBRARY_PREFIX="${CMAKE_SHARED_LIBRARY_PREFIX}"
)

if(WINDOWS)
    set(COMPILE_DEFINITIONS
			${COMPILE_DEFINITIONS}
            S2SDK_PLATFORM="win64"
            S2SDK_ROOT_BINARY="/bin/win64/"
            S2SDK_GAME_BINARY="/csgo/bin/win64/"
    )
elseif(LINUX)
    set(COMPILE_DEFINITIONS
            ${COMPILE_DEFINITIONS}
            S2SDK_PLATFORM="linuxsteamrt64"
            S2SDK_ROOT_BINARY="/bin/linuxsteamrt64/"
            S2SDK_GAME_BINARY="/bin/linuxsteamrt64/../../csgo/bin/linuxsteamrt64/" # Metamod hack
    )
endif()

file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

set(INCLUDE_DIRS
		${EXPORTS_BINARY_DIR}
		${SOURCE_DIR}
)

set(PRECOMPILER_HEADERS
		${SOURCE_DIR}/pch.h
)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

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
elseif(MACOS)
    set_target_properties(${PROJECT_NAME} PROPERTIES OSX_ARCHITECTURES "x86_64")
endif()

target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILER_OPTIONS} ${SOURCESDK_COMPILE_OPTIONS} ${PLUGIFY_COMPILE_OPTIONS})

generate_export_header(${PROJECT_NAME} EXPORT_MACRO_NAME PLUGIN_API EXPORT_FILE_NAME ${EXPORTS_BINARY_DIR}/plugin_export.h)
target_precompile_headers(${PROJECT_NAME} PRIVATE ${PRECOMPILER_HEADERS})

target_compile_definitions(${PROJECT_NAME} PRIVATE ${COMPILE_DEFINITIONS} ${METAMOD_COMPILE_DEFINITIONS} ${PLUGIFY_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS} ${DYNLIBUTILS_INCLUDE_DIRS} ${METAMOD_INCLUDE_DIRS} ${PLUGIFY_CONFIGS_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE cpp-memory_utils ${RE2_LINK_LIBRARIES} ${PLUGIFY_LINK_LIBRARIES} ${PLUGIFY_CONFIGS_BINARY_DIR} ${SOURCESDK_BINARY_DIR})

if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-Wl,-exported_symbols_list,${CMAKE_CURRENT_SOURCE_DIR}/sym/exported_symbols.lds")
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-Wl,--version-script,${CMAKE_CURRENT_SOURCE_DIR}/sym/version_script.lds")
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/s2sdk.pplugin.in ${CMAKE_CURRENT_BINARY_DIR}/s2sdk.pplugin)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/assets/gamedata.jsonc ${CMAKE_CURRENT_BINARY_DIR}/gamedata.jsonc COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/assets/settings.jsonc ${CMAKE_CURRENT_BINARY_DIR}/settings.jsonc COPYONLY)

