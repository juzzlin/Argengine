cmake_minimum_required(VERSION 3.10)
cmake_policy(VERSION 3.10)

project(Argengine)

option(BUILD_EXAMPLES "Build example apps" ON)

option(BUILD_TESTS "Build unit tests" ON)

# Default to release C++ flags if CMAKE_BUILD_TYPE not set
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo
MinSizeRel." FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_COMPILER_IS_GNUCXX OR MINGW OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

set(LIBRARY_NAME "Argengine")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(src/tests)
endif()

add_subdirectory(src)

