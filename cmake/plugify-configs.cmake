include(FetchContent)

message(STATUS "Pulling and configuring plugify-configs")

FetchContent_Declare(
        plugify-configs
        GIT_REPOSITORY https://github.com/untrustedmodders/plugify-configs.git
		GIT_TAG 928a797f3ccd254d4aa79b4415bf4bfc70d8fc02
)

FetchContent_MakeAvailable(plugify-configs)
