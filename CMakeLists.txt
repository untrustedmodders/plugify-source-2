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
elseif(WIN32)
    include("cmake/windows.cmake")
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "cs2sdk")

target_compile_definitions(${PROJECT_NAME} PRIVATE
        CS2SDK_LOGGING_PREFIX="cs2-sdk"
        CS2SDK_PLATFORM_WINDOWS=$<BOOL:${WIN32}>
        CS2SDK_PLATFORM_APPLE=$<BOOL:${APPLE}>
        CS2SDK_PLATFORM_LINUX=$<BOOL:${LINUX}>
        CS2SDK_LIBRARY_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}"
        CS2SDK_LIBRARY_PREFIX="${CMAKE_SHARED_LIBRARY_PREFIX}")

# Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${CS2SDK_LINK_LIBRARIES})

# TODO: Finish that
#if(APPLE)
#	target_link_libraries(${PROJECT_NAME} PRIVATE "-Wl,-exported_symbols_list,${CMAKE_CURRENT_SOURCE_DIR}/sym/exported_symbols.lds")
#elseif(UNIX)
#	target_link_libraries(${PROJECT_NAME} PRIVATE "-Wl,--version-script,${CMAKE_CURRENT_SOURCE_DIR}/sym/version_script.lds")
#endif()

if (LINIX)
    target_compile_options(${PROJECT_NAME} PUBLIC -stdlib=libstdc++)
    target_link_libraries(${PROJECT_NAME} PUBLIC -static-libstdc++ -static-libgcc)
endif()

set(CS2SDK_VERSION "${CMAKE_PROJECT_VERSION}" CACHE STRING "Set version name")
set(CS2SDK_PACKAGE "cs2sdk" CACHE STRING "Set package name")

configure_file(
        ${CMAKE_SOURCE_DIR}/cs2sdk.pplugin.in
        ${CMAKE_BINARY_DIR}/cs2sdk.pplugin
)