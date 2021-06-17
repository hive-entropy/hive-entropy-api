add_executable(rowcol
    main.cpp
)

target_compile_options(rowcol PRIVATE -Wall)
target_compile_features(rowcol PRIVATE cxx_std_17)

target_link_libraries(rowcol
    PRIVATE HiveEntropyAPI::HiveEntropyAPI
)
