include(FetchContent)

message(STATUS "Pulling and configuring plugify-configs")

FetchContent_Declare(
        plugify-configs
        GIT_REPOSITORY https://github.com/untrustedmodders/plugify-configs.git
		GIT_TAG 6c5bfceab8fd907fb3379fd9f4543cbc84db08f7
		GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(plugify-configs)
