macro(set_common_compile_definitions TARGET_NAME)
    target_compile_definitions(${TARGET_NAME} PRIVATE
            META_IS_SOURCE2
            _LINUX
            POSIX
            LINUX
            GNUC
            COMPILER_GCC
            PLATFORM_64BITS
            _GLIBCXX_USE_CXX11_ABI=0
    )
endmacro()

macro(set_additional_compile_definitions TARGET_NAME)
    target_compile_definitions(${TARGET_NAME} PRIVATE
            stricmp=strcasecmp
            _stricmp=strcasecmp
            _strnicmp=strncasecmp
            strnicmp=strncasecmp
            _snprintf=snprintf
            _vsnprintf=vsnprintf
            _alloca=alloca
            strcmpi=strcasecmp
    )
endmacro()

macro(set_compile_options TARGET_NAME)
    target_compile_options(${TARGET_NAME} PRIVATE
            -Wall
            -Wno-uninitialized
            -Wno-switch
            -Wno-unused
            -Wno-non-virtual-dtor
            -Wno-overloaded-virtual
            -Wno-conversion-null
            -Wno-write-strings
            -Wno-invalid-offsetof
            -Wno-reorder
            -mfpmath=sse
            -msse
            -fno-strict-aliasing
            -fno-threadsafe-statics
            -v
            -fvisibility=default
    )
endmacro()

set_common_compile_definitions(${PROJECT_NAME})
set_additional_compile_definitions(${PROJECT_NAME})
set_compile_options(${PROJECT_NAME})

set_common_compile_definitions(libprotobuf)
set_additional_compile_definitions(libprotobuf)
set_compile_options(libprotobuf)

set(CS2SDK_LINK_LIBRARIES
        ${SOURCESDK_LIB}/linux64/libtier0.so
        #${SOURCESDK_LIB}/linux64/tier1.a
        #${SOURCESDK_LIB}/linux64/interfaces.a
        #${SOURCESDK_LIB}/linux64/mathlib.a
        #${SOURCESDK_LIB}/linux64/release/libprotobuf.a
        protobuf::libprotobuf
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        CS2SDK_PLATFORM="linux"
        CS2SDK_BINARY="linuxsteamrt64"
        CS2SDK_ROOT_BINARY="/bin/linuxsteamrt64/"
        # Metamod hack
        CS2SDK_GAME_BINARY="/bin/linuxsteamrt64/../../csgo/bin/linuxsteamrt64/"
)

# TODO: Finish that
#target_link_libraries(${PROJECT_NAME} PRIVATE "-Wl,--version-script,${CMAKE_CURRENT_SOURCE_DIR}/sym/version_script.lds")

target_link_libraries(${PROJECT_NAME} PUBLIC -static-libstdc++ -static-libgcc)
