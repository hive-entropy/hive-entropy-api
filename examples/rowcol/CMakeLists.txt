find_package(HiveEntropyAPI REQUIRED)

add_executable(rowcol
    main.cpp
)

target_compile_options(rowcol PRIVATE -Wall)
target_compile_features(rowcol PRIVATE cxx_std_11)

target_link_libraries(rowcol
    PRIVATE HiveEntropyAPI::HiveEntropyAPI
)
