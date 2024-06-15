if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(WIN32 AND NOT MSVC)
    message(FATAL "MSVC restricted.")
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING
    "Only do Release and Debug"
    FORCE
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(SOURCESDK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/hl2sdk-cs2)
set(METAMOD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/metamod-source)

set(SOURCESDK ${SOURCESDK_DIR}/${BRANCH})
set(SOURCESDK_LIB ${SOURCESDK}/lib)

include_directories(
        ${SOURCESDK}
        ${SOURCESDK}/thirdparty/protobuf-3.21.8/src
        ${SOURCESDK}/common
        ${SOURCESDK}/game/shared
        ${SOURCESDK}/game/server
        ${SOURCESDK}/public
        ${SOURCESDK}/public/engine
        ${SOURCESDK}/public/mathlib
        ${SOURCESDK}/public/tier0
        ${SOURCESDK}/public/tier1
        ${SOURCESDK}/public/entity2
        ${SOURCESDK}/public/game/server
        ${SOURCESDK}/public/entity2
        ${METAMOD_DIR}/core
        ${METAMOD_DIR}/core/sourcehook
)

set(protobuf_INSTALL OFF CACHE BOOL "Install protobuf binaries and files")
set(protobuf_BUILD_TESTS OFF CACHE BOOL "Build tests")
add_subdirectory(${SOURCESDK}/thirdparty/protobuf-3.21.8)
set_target_properties(libprotobuf PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
