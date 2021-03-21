# See cross-compile guide : https://kubasejdak.com/how-to-cross-compile-for-embedded-with-cmake-like-a-champ
# https://dev.to/younup/cmake-on-stm32-episode-5-get-ready-for-c-1mh9
# https://github.com/modm-io/modm

# Set minimum cmake version
cmake_minimum_required(VERSION 3.17)
# Set project name
project(PDS)

# Set compiler standard to C++11
set(CMAKE_CXX_STANDARD 11)

# Set in which directories cmake will search for headers
include_directories(include)

# Set the list of source files
set(SOURCE_FILES
        src/main.cpp
        src/Client.cpp
        src/DistributionHandler.cpp
        src/Matrix.cpp
        src/ResultHandler.cpp
        src/Server.cpp
        src/Socket.cpp
        src/Validator.cpp)

# Find the OpenBlas package
find_package(BLAS REQUIRED)
message("OpenBLAS found.")
find_path(BLAS_INCLUDE_DIRS cblas.h
        /usr/include
        /usr/local/include
        $ENV{BLAS_HOME}/include)
message(${BLAS_LIBRARIES})
message(${BLAS_INCLUDE_DIRS})
include_directories(${BLAS_INCLUDE_DIRS})

# Create the executable
add_executable(PDS ${SOURCE_FILES})

add_subdirectory(lib/OpenBLAS/)

# Link the OpenBlas library to the target
target_link_libraries(PDS openblas${SUFFIX64_UNDERSCORE})