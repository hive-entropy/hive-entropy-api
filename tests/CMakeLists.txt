add_subdirectory(lib/Catch2)

add_executable (validation
    src/tests.cpp
    src/matrix_tests.cpp
)

target_link_libraries(validation PRIVATE HiveEntropyAPI Catch2::Catch2)
target_include_directories(validation PUBLIC include())