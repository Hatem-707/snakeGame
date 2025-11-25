cmake_minimum_required(VERSION 3.11)
project(snakeGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")

# include(FetchContent)
# set(FETCHCONTENT_QUIET FALSE)
# set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
# set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)

# FetchContent_Declare(
#     raylib
#     GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
#     GIT_TAG "master"
#     GIT_PROGRESS TRUE
# )

# FetchContent_MakeAvailable(raylib)

find_package(raylib REQUIRED)

add_executable(snakeGame ${SOURCE_DIR}/main.cpp)
target_link_libraries(snakeGame PRIVATE raylib)
