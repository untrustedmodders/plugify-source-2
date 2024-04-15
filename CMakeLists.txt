cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

if(POLICY CMP0092)
    cmake_policy(SET CMP0092 NEW) # Don't add -W3 warning level by default.
endif()
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()


project(cs2-sdk-plugin VERSION 1.0.0.0 DESCRIPTION "CS2 SDK Plugin" HOMEPAGE_URL "https://github.com/untrustedmodders/cs2-sdk-plugin" LANGUAGES C CXX ASM)

include("cmake/shared.cmake")

file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

set(SOURCE_FILES ${SOURCE_FILES}
        ${SOURCESDK_DIR}/tier1/convar.cpp
        ${SOURCESDK_DIR}/tier1/generichash.cpp
        #${SOURCESDK_DIR}/tier1/keyvalues3.cpp
        ${SOURCESDK_DIR}/entity2/entityidentity.cpp
        ${SOURCESDK_DIR}/entity2/entitysystem.cpp
        ${SOURCESDK_DIR}/entity2/entitykeyvalues.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/external/dynlibutils/module.cpp
)

if (LINUX)
    # memoverride.cpp is not usable on CMake Windows, because CMake default link libraries always link ucrt.lib
    set(SOURCE_FILES ${SOURCE_FILES}
            ${SOURCESDK_DIR}/public/tier0/memoverride.cpp
    )
endif()

## Generate protobuf source & headers
if (LINUX)
    set(PROTOC_EXECUTABLE ${SOURCESDK_DIR}/devtools/bin/linux/protoc)
elseif(WIN32)
    set(PROTOC_EXECUTABLE ${SOURCESDK_DIR}/devtools/bin/protoc.exe)
endif()

execute_process(
        COMMAND ${PROTOC_EXECUTABLE} --proto_path=thirdparty/protobuf-3.21.8/src --proto_path=common --proto_path=game/shared/ --cpp_out=common networkbasetypes.proto network_connection.proto usermessages.proto
        WORKING_DIRECTORY ${SOURCESDK_DIR}
)

set(SOURCE_FILES ${SOURCE_FILES}
        ${SOURCESDK_DIR}/common/networkbasetypes.pb.cc
        ${SOURCESDK_DIR}/common/network_connection.pb.cc
        ${SOURCESDK_DIR}/common/usermessages.pb.cc
)

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
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            CS2SDK_PLATFORM="linux"
            CS2SDK_BINARY="linuxsteamrt64"
            CS2SDK_ROOT_BINARY="/bin/linuxsteamrt64/"
            CS2SDK_GAME_BINARY="/csgo/bin/linuxsteamrt64/"
    )
elseif(WIN32)
    include("cmake/windows.cmake")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            CS2SDK_PLATFORM="windows"
            CS2SDK_BINARY="win64"
            CS2SDK_ROOT_BINARY="/bin/win64/"
            CS2SDK_GAME_BINARY="/csgo/bin/win64/"
    )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
		PREFIX ""
        OUTPUT_NAME "cs2sdk"
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/addons/plugify/plugins/cs2sdk/bin"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/addons/plugify/plugins/cs2sdk/bin"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/addons/plugify/plugins/cs2sdk/bin"

)

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
