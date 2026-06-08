cmake_minimum_required(VERSION 3.10)
project(my_stonefish_sim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(stonefish)

add_executable(tmr-simulator
    src/main.cpp
    src/SimulationManager.cpp
    src/MyApp.cpp
)

set_target_properties(tmr-simulator PROPERTIES
    OUTPUT_NAME "tmr-simulator"
)

target_include_directories(tmr-simulator BEFORE PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/stonefish
    ${CMAKE_CURRENT_SOURCE_DIR}/stonefish/Library/include
    ${CMAKE_CURRENT_SOURCE_DIR}/stonefish/3rdparty
)

target_compile_definitions(tmr-simulator PRIVATE DATA_DIR="${CMAKE_SOURCE_DIR}/data")

target_link_libraries(tmr-simulator PRIVATE Stonefish)