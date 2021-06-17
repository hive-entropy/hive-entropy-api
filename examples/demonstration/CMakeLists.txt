add_executable(demo
        demo.cpp
        )

# Find OpenCV
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV_INCLUDE_DIRS = ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV_LIBS = ${OpenCV_LIBS}")

include_directories(${OpenCV_INCLUDE_DIRS} "/usr/include/opencv4/opencv2" )

# Add target options and features
target_compile_options(demo PRIVATE -Wall -Wextra -Wconversion -pedantic)
target_compile_features(demo PRIVATE cxx_std_17)

# Link the libraries
target_link_libraries(demo
        PRIVATE HiveEntropyAPI::HiveEntropyAPI
        PRIVATE ${OpenCV_LIBS}
        )