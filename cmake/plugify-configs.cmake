include(FetchContent)

message(STATUS "Pulling and configuring plugify-configs")

FetchContent_Declare(
        plugify-configs
        GIT_REPOSITORY https://github.com/untrustedmodders/plugify-configs.git
		GIT_TAG cecc32cf7f70dbd53998c55784339c86c5a71e18
)

FetchContent_MakeAvailable(plugify-configs)
