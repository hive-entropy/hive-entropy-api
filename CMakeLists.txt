cmake_minimum_required(VERSION 3.17)

set (CMAKE_CXX_STANDARD 17)

project(HiveEntropyAPI VERSION 0.9.0 LANGUAGES CXX)

option(BUILD_EXAMPLES "Build Examples" ON)

# Add the cmake folder so the FindSphinx module is found
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

enable_testing()

add_subdirectory(heapi)
add_subdirectory(tests)
add_subdirectory(doc)

if(BUILD_EXAMPLES)
add_subdirectory(examples)
endif(BUILD_EXAMPLES)