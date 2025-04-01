include(FetchContent)

message(STATUS "Pulling and configuring plugify-configs")

FetchContent_Declare(
        plugify-configs
        GIT_REPOSITORY https://github.com/untrustedmodders/plugify-configs.git
		GIT_TAG a65ec9fd591610ed85523579b717ecd6d9d81c5a
)

FetchContent_MakeAvailable(plugify-configs)
