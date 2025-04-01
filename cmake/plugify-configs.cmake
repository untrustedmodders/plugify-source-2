include(FetchContent)

message(STATUS "Pulling and configuring plugify-configs")

FetchContent_Declare(
        plugify-configs
        GIT_REPOSITORY https://github.com/untrustedmodders/plugify-configs.git
		GIT_TAG 28218969327160ba85a66164e4295b03e2c497f6
)

FetchContent_MakeAvailable(plugify-configs)
