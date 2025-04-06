include(FetchContent)

message(STATUS "Pulling and configuring plugify-configs")

FetchContent_Declare(
        plugify-configs
        GIT_REPOSITORY https://github.com/untrustedmodders/plugify-configs.git
		GIT_TAG 34ccabb7e45b7fa2063a4dcd2084d0ef6063402a
		GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(plugify-configs)
