# See cross-compile guide : https://kubasejdak.com/how-to-cross-compile-for-embedded-with-cmake-like-a-champ
# https://dev.to/younup/cmake-on-stm32-episode-5-get-ready-for-c-1mh9
# https://github.com/modm-io/modm

cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_FLAGS "-lpthread -Wall -g ")

# Add the library project to be build
add_subdirectory(lib/OpenBLAS/)
add_subdirectory(lib/libcoap/)

# Set in which directories cmake will search for headers
include_directories(include)
# Add include directory for cblas.h
#file(INSTALL lib/OpenBLAS/cblas.h DESTINATION ${PROJECT_SOURCE_DIR}/lib) # Copy cblas.h to lib to prevent include of other headers
#include_directories(lib)

# Create the executable
add_library(heapi STATIC 
    src/IncompatibleDimensionsException.cpp 
    src/OutOfBoundException.cpp
    src/Message.cpp
    src/CoapEndpoint.cpp
    src/HiveEntropyNode.cpp
    src/ResponseBuilder.cpp
)

# Link the OpenBlas library to the target
# See for static linking : https://stackoverflow.com/a/48966279/15249616
target_link_libraries(heapi 
    openblas${SUFFIX64_UNDERSCORE}
    coap-2
)

set_target_properties(
    heapi PROPERTIES
    VERSION 0.1
)

install(
    TARGETS heapi
    LIBRARY
)

install(
    DIRECTORY include/ DESTINATION include/hive_entropy
        FILES_MATCHING PATTERN "*.h"
)