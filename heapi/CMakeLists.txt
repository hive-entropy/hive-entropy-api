##############################################
# CMake documentation

# Effective Modern CMake. Overall on CMake usage:
# https://gist.github.com/mbinna/c61dbb39bca0e4fb7d1f73b0d66a4fd1

# Correct ways to use third party libraries:
# https://stackoverflow.com/questions/51564251/correct-way-to-use-third-party-libraries-in-cmake-project
# FetchContent usage for GitHub libraries:
# https://medium.com/analytics-vidhya/c-dependency-management-with-cmakes-fetchcontent-4ceca4693a5d

# Use CMake to build, install and export a library:
# https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/
# https://cmake.org/cmake/help/git-stage/guide/importing-exporting/index.html
# Written based on this model:
# https://github.com/pabloariasal/modern-cmake-sample/blob/master/libjsonutils/CMakeLists.txt

# Cross-compilation guide : https://kubasejdak.com/how-to-cross-compile-for-embedded-with-cmake-like-a-champ
# https://dev.to/younup/cmake-on-stm32-episode-5-get-ready-for-c-1mh9
# https://github.com/modm-io/modm

##############################################
# External project dependencies

include(FetchContent)

FetchContent_Declare(OpenBLAS
        GIT_REPOSITORY git@github.com:xianyi/OpenBLAS.git
        GIT_TAG 2f6d35c3d462c996b6504cc0ddc2722e36dd2a71
        GIT_PROGRESS ON
        )

FetchContent_Declare(libcoap
        GIT_REPOSITORY git@github.com:obgm/libcoap.git
        GIT_TAG 4cab59dbbf3048e44220ff2e7d6b36284b3ef131
        GIT_PROGRESS ON
        )

# Disable the CMP0048 policy warning
file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/EnableCMP0048.cmake" "cmake_policy(SET CMP0048 NEW)")
set(CMAKE_PROJECT_INCLUDE_BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/EnableCMP0048.cmake")

FetchContent_MakeAvailable(OpenBLAS libcoap) # Make the libs available

unset(CMAKE_PROJECT_INCLUDE_BEFORE)

##############################################
# Find system dependencies
set(CMAKE_THREAD_PREFER_PTHREAD ON)
find_package(Threads REQUIRED)

##############################################
# Create target and set properties

add_library(HiveEntropyAPI
        STATIC
        src/IncompatibleDimensionsException.cpp
        src/OutOfBoundException.cpp
        )

# Set in which directories cmake will search for headers
# PUBLIC headers should be put in include directory.
# Whereas PRIVATE headers should be put inside src.
target_include_directories(HiveEntropyAPI
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include/HiveEntropyAPI>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

# Set the compiler to use C++11
target_compile_features(HiveEntropyAPI
        PRIVATE cxx_std_11
        )

target_compile_options(HiveEntropyAPI
        PRIVATE -Wall -g
        )

# Link the OpenBlas library to the target
# See for static linking : https://stackoverflow.com/a/48966279/15249616
# Dependency should be made public ONLY if it is used in a public header
target_link_libraries(HiveEntropyAPI
    PRIVATE OpenBLAS libcoap Threads::Threads
)

##############################################
# Installation instructions

# Make cache variables for install destinations
include(GNUInstallDirs)

# Install target and create export-set
install(TARGETS HiveEntropyAPI
        EXPORT HiveEntropyAPITargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # For windows
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # Static library
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # Shared library
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install header files
file(GLOB_RECURSE PUBLIC_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h*")
install(FILES ${PUBLIC_HEADER_FILES}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/HiveEntropyAPI
)

# Generate and install export file
install(EXPORT HiveEntropyAPITargets
        FILE HiveEntropyAPITargets.cmake
        NAMESPACE HiveEntropyAPI::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/HiveEntropyAPI
)

# Include CMakePackageConfigHelpers macro
include(CMakePackageConfigHelpers)

# generate the version file for the config file
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/HiveEntropyAPIConfigVersion.cmake"
        VERSION "${PROJECT_VERSION}"
        COMPATIBILITY AnyNewerVersion
)

# Create the config file
configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/HiveEntropyAPIConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/HiveEntropyAPIConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/HiveEntropyAPI
)

#Install the config, configversion and custom find modules
install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/HiveEntropyAPIConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/HiveEntropyAPIConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/HiveEntropyAPI
)

##############################################
## Exporting from the build tree
message(STATUS "Exported HiveEntropyAPITargets.cmake to ${CMAKE_CURRENT_BINARY_DIR}.")
export(EXPORT HiveEntropyAPITargets
        FILE "${CMAKE_CURRENT_BINARY_DIR}/HiveEntropyAPITargets.cmake"
        NAMESPACE HiveEntropyAPI::
)