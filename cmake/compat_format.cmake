# plugify-source-2
# Copyright (C) 2023-2025 untrustedmodders
# Licensed under the MIT license. See LICENSE file in the project root for details.

include(CheckCXXSourceCompiles)
include(CMakePushCheckState)

cmake_push_check_state()

check_cxx_source_compiles("
    #include<format>
    #include<string>
    int main() { std::string a = std::format(\"{}.{}.{}\", \"Hello\", \"World\", \"C++\"); return 0; }
    " COMPILER_SUPPORTS_FORMAT)

cmake_pop_check_state()

if(NOT COMPILER_SUPPORTS_FORMAT)
    include(cmake/fmt.cmake)
endif()
