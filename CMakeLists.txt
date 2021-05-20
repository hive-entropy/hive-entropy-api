cmake_minimum_required(VERSION 3.17)

project(HiveEntropyAPI VERSION 0.1.1 LANGUAGES CXX)

option(BUILD_EXAMPLES "Build Examples" ON)


enable_testing()

add_subdirectory(heapi)
add_subdirectory(tests)

if(BUILD_EXAMPLES)
add_subdirectory(examples)
endif(BUILD_EXAMPLES)