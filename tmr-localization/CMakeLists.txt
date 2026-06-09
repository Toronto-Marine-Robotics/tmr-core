cmake_minimum_required(VERSION 3.10)

set(CMAKE_POLICY_VERSION_MINIMUM "3.5" CACHE STRING "" FORCE)

project(tmr-localization)


# Set C++ standard (GTSAM usually requires C++14 or higher)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Configure GTSAM options to ensure it builds correctly as a subproject
# Disable GTSAM's tests, examples, and wrappers to save build time
set(GTSAM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GTSAM_BUILD_EXAMPLES_ALWAYS OFF CACHE BOOL "" FORCE)
set(GTSAM_BUILD_WRAPPER OFF CACHE BOOL "" FORCE)

# 2. Add the subdirectory
# This looks into tmr-localization/gtsam/CMakeLists.txt
cmake_policy(SET CMP0048 NEW)
add_subdirectory(gtsam)

# 3. Define your own target
add_executable(tmr-localization src/main.cpp)

# 4. Link against GTSAM
# 'gtsam' is the library name exported by the GTSAM CMake files
target_link_libraries(tmr-localization PRIVATE gtsam)

# 5. Include headers
target_include_directories(tmr-localization PRIVATE gtsam)