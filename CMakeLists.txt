cmake_minimum_required(VERSION 3.22)

project(led_sandbox
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

include(FetchContent)

FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG v3.4.0
)

FetchContent_MakeAvailable(tomlplusplus)

add_executable(sandbox
    src/main.cpp
    src/sandbox/Application.cpp
    src/sandbox/ApplicationConfig.cpp
    src/sandbox/SceneParser.cpp
    src/sandbox/SceneFolderMonitor.cpp
    src/sandbox/LedDisplay.cpp
)

set(RGB_MATRIX_DIR "/home/ggraves/github/rpi-rgb-led-matrix")

target_include_directories(sandbox
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${RGB_MATRIX_DIR}/include
)

target_link_libraries(sandbox
    PRIVATE
        CURL::libcurl
        nlohmann_json::nlohmann_json
        tomlplusplus::tomlplusplus
        ${RGB_MATRIX_DIR}/lib/librgbmatrix.a
)