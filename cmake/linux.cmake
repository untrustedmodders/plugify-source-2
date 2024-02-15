add_definitions(-D_LINUX -DPOSIX -DLINUX -DGNUC -DCOMPILER_GCC -DPLATFORM_64BITS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dstricmp=strcasecmp -D_stricmp=strcasecmp -D_strnicmp=strncasecmp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dstrnicmp=strncasecmp -D_snprintf=snprintf")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_vsnprintf=vsnprintf -D_alloca=alloca -Dstrcmpi=strcasecmp")

# Warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-uninitialized -Wno-switch -Wno-unused")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-non-virtual-dtor -Wno-overloaded-virtual")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-conversion-null -Wno-write-strings")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof -Wno-reorder")

# Others
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpmath=sse -msse -fno-strict-aliasing")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-threadsafe-statics -v -fvisibility=default")

set(CS2SDK_LINK_LIBRARIES
		${SOURCESDK_LIB}/linux64/libtier0.so
		${SOURCESDK_LIB}/linux64/tier1.a
		${SOURCESDK_LIB}/linux64/interfaces.a
		#${SOURCESDK_LIB}/linux64/mathlib.a
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
		CS2SDK_PLATFORM="linux"
		CS2SDK_BINARY="linuxsteamrt64"
		CS2SDK_ROOT_BINARY="/bin/linuxsteamrt64/"
		CS2SDK_GAME_BINARY="/csgo/bin/linuxsteamrt64/"
)