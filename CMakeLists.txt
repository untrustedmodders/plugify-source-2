cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

if(POLICY CMP0092)
    cmake_policy(SET CMP0092 NEW) # Don't add -W3 warning level by default.
endif()


project(cs2-sdk-plugin VERSION 1.0.0.0  DESCRIPTION "CS2 SDK Plugin" HOMEPAGE_URL "https://github.com/untrustedmodders/cs2-sdk-plugin" LANGUAGES C CXX ASM)

include("cmake/shared.cmake")

file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

set(SOURCE_FILES ${SOURCE_FILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/external/hl2sdk-cs2/tier1/convar.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/external/hl2sdk-cs2/tier1/generichash.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/external/hl2sdk-cs2/tier1/keyvalues3.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/external/hl2sdk-cs2/entity2/entityidentity.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/external/hl2sdk-cs2/entity2/entitysystem.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/external/hl2sdk-cs2/entity2/entitykeyvalues.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/external/dynlibutils/module.cpp
)

if (LINUX)
    # memoverride.cpp is not usable on CMake Windows, because CMake default link libraries always link ucrt.lib
    set(SOURCE_FILES ${SOURCE_FILES}
            ${CMAKE_CURRENT_SOURCE_DIR}/external/hl2sdk-cs2/public/tier0/memoverride.cpp
    )
endif()

## Generate protobuf source & headers
if (LINUX)
    set(PROTOC_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/external/hl2sdk-cs2/devtools/bin/linux/protoc)
elseif(WIN32)
    set(PROTOC_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/external/hl2sdk-cs2/devtools/bin/protoc.exe)
endif()

add_custom_command(
        OUTPUT protobuf_output_stamp
        COMMAND ${PROTOC_EXECUTABLE} --proto_path=thirdparty/protobuf-3.21.8/src --proto_path=common --cpp_out=common common/network_connection.proto
        COMMENT "Generating protobuf file"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/hl2sdk-cs2
        VERBATIM
)

SET(SOURCE_FILES ${SOURCE_FILES} protobuf_output_stamp)


# Sources
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${NATIVES_SOURCES} ${CONVERSIONS_SOURCES} ${CONVERSIONS_HEADERS})

target_precompile_headers(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h)

target_include_directories(
        ${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/cs2_sdk
        ${CMAKE_CURRENT_SOURCE_DIR}/external/dynlibutils
)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME} EXPORT_MACRO_NAME PLUGIN_API EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/plugin_export.h)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/exports)

if(LINUX)
    include("cmake/linux.cmake")
elseif(WIN32)
    include("cmake/windows.cmake")
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
		PREFIX ""
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/addons/plugify/plugins/cs2sdk/bin"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/addons/plugify/plugins/cs2sdk/bin"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/addons/plugify/plugins/cs2sdk/bin"
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "cs2sdk")

# Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${CS2SDK_LINK_LIBRARIES})

target_compile_definitions(${PROJECT_NAME} PRIVATE
        CS2SDK_LOGGING_PREFIX="cs2-sdk"
        CS2SDK_PLATFORM_WINDOWS=$<BOOL:${WIN32}>
        CS2SDK_PLATFORM_APPLE=$<BOOL:${APPLE}>
        CS2SDK_PLATFORM_LINUX=$<BOOL:${UNIX}>
        BINARY_MODULE_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}"
        BINARY_MODULE_PREFIX="${CMAKE_SHARED_LIBRARY_PREFIX}")

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy 
        ${CMAKE_SOURCE_DIR}/cs2sdk.pplugin ${CMAKE_BINARY_DIR}/addons/plugify/plugins/cs2sdk
)

# Temp

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory
        D:/steamcmd/cs2-ds/game/csgo/addons/plugify/plugins/cs2sdk
)

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_BINARY_DIR}/addons/plugify/plugins/cs2sdk
        D:/steamcmd/cs2-ds/game/csgo/addons/plugify/plugins/cs2sdk
)
