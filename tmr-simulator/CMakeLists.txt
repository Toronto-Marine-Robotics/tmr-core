cmake_minimum_required(VERSION 3.10)
project(my_stonefish_sim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(stonefish)

# --- Zenoh C ---
set(ZENOH_C_DIR "$ENV{HOME}/zenoh-c"
    CACHE PATH "Path to zenoh-c SDK")

add_library(zenohc_lib STATIC IMPORTED)
set_target_properties(zenohc_lib PROPERTIES
    IMPORTED_LOCATION "${ZENOH_C_DIR}/lib/libzenohc.a"
)

add_library(zenohc INTERFACE)
target_include_directories(zenohc INTERFACE "${ZENOH_C_DIR}/include")
target_link_libraries(zenohc INTERFACE zenohc_lib)
target_link_libraries(zenohc INTERFACE "-lm -ldl -pthread")

# --- Executable ---
add_executable(tmr-simulator
    src/main.cpp
    src/SimulationManager.cpp
    src/MyApp.cpp
    src/Telemetry.cpp
)

set_target_properties(tmr-simulator PROPERTIES
    OUTPUT_NAME "tmr-simulator"
)

target_include_directories(tmr-simulator BEFORE PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/stonefish/Library/include
    ${CMAKE_BINARY_DIR}/stonefish
    ${CMAKE_CURRENT_SOURCE_DIR}/stonefish/3rdparty
)

target_compile_definitions(tmr-simulator PRIVATE DATA_DIR="${CMAKE_SOURCE_DIR}/data")

target_link_libraries(tmr-simulator PRIVATE
    Stonefish
    zenohc
)
