add_subdirectory(lib/Catch2)

add_executable (validation src/tests.cpp)

target_link_libraries(validation PRIVATE heapi Catch2::Catch2)
target_include_directories(validation PUBLIC include())