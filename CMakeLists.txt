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

# Ubuntu's cppzmq package provides this fallback FindZeroMQ.cmake module.
list(APPEND CMAKE_MODULE_PATH
    "/usr/share/cmake/cppzmq/libzmq-pkg-config"
)
find_package(ZeroMQ REQUIRED)
find_package(cppzmq REQUIRED)

include(FetchContent)

FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG v3.4.0
)

FetchContent_MakeAvailable(tomlplusplus)

FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.2.1.2
)

set(CROW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CROW_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(Crow)

add_executable(sandbox
    src/main.cpp
    src/sandbox/Application.cpp
    src/sandbox/SceneParser.cpp
    src/sandbox/SceneFolderMonitor.cpp
    src/sandbox/LedDisplay.cpp
    src/sandbox/comms/DisplayIpcServer.cpp
    src/sandbox/comms/RestApiServer.cpp
    src/sandbox/config/ApplicationConfig.cpp
)

set(RGB_MATRIX_DIR "/home/ggraves/github/rpi-rgb-led-matrix")

target_include_directories(sandbox
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${RGB_MATRIX_DIR}/include
)

target_link_libraries(sandbox
    PRIVATE
        nlohmann_json::nlohmann_json
        tomlplusplus::tomlplusplus
        cppzmq
        Crow::Crow
        ${RGB_MATRIX_DIR}/lib/librgbmatrix.a
)