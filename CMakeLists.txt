cmake_minimum_required(VERSION 3.17)
project(HiveEntropyAPI VERSION 0.9.0 LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 17)

# Use --log-level=<ERROR|WARNING|NOTICE|STATUS|VERBOSE|DEBUG|TRACE> to change log level when building with cmake
set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE) # Display verbose messages

option(BUILD_EXAMPLES "Build Examples" ON)

# Add the cmake folder so the FindSphinx module is found
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
message(VERBOSE "Added module path ${CMAKE_MODULE_PATH}\n")

enable_testing()

add_subdirectory(heapi)
add_subdirectory(tests)
add_subdirectory(doc/config)

if(BUILD_EXAMPLES)
add_subdirectory(examples)
endif(BUILD_EXAMPLES)