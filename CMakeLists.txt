# See cross-compile guide : https://kubasejdak.com/how-to-cross-compile-for-embedded-with-cmake-like-a-champ
# https://dev.to/younup/cmake-on-stm32-episode-5-get-ready-for-c-1mh9
# https://github.com/modm-io/modm

# Set minimum cmake version
cmake_minimum_required(VERSION 3.17)
# Set compiler standard to C++11
set(CMAKE_CXX_STANDARD 11)
# Compilation Flags
set(CMAKE_C_FLAGS " -Wall -g ")
# Set project name
project(PDS)

# Add the library project to be build
add_subdirectory(lib/OpenBLAS/)

# Set in which directories cmake will search for headers
include_directories(include)
# Add include directory for cblas.h
#file(INSTALL lib/OpenBLAS/cblas.h DESTINATION ${PROJECT_SOURCE_DIR}/lib) # Copy cblas.h to lib to prevent include of other headers
#include_directories(lib)

# Set the list of source files
# Source files should be added manually (or by the IDE), not automatically detected
# For details : https://stackoverflow.com/a/57928919/15249616
set(SOURCE_FILES src/main.cpp src/Matrix.cpp)
#file( GLOB SOURCE_FILES src/*.cpp )

# Create the executable
add_executable(PDS ${SOURCE_FILES})

# Link the OpenBlas library to the target
# See for static linking : https://stackoverflow.com/a/48966279/15249616
target_link_libraries(PDS openblas${SUFFIX64_UNDERSCORE})