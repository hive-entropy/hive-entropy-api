add_executable(boomerang
    main.cpp
)

target_compile_options(boomerang PRIVATE -Wall)
target_compile_features(boomerang PRIVATE cxx_std_17)

target_link_libraries(boomerang
    PRIVATE HiveEntropyAPI::HiveEntropyAPI
)
