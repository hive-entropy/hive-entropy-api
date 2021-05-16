cmake_minimum_required(VERSION 3.17)

project(HiveEntropyAPI VERSION 0.1.1 LANGUAGES CXX)

enable_testing()

add_subdirectory(heapi)
add_subdirectory(tests)
add_subdirectory(examples)