##############################################
# CMake documentation

# Effective Modern CMake. Overall on CMake usage:
# https://gist.github.com/mbinna/c61dbb39bca0e4fb7d1f73b0d66a4fd1

# Correct ways to use third party libraries:
# https://stackoverflow.com/questions/51564251/correct-way-to-use-third-party-libraries-in-cmake-project
# FetchContent usage for GitHub libraries:
# https://medium.com/analytics-vidhya/c-dependency-management-with-cmakes-fetchcontent-4ceca4693a5d
# Handle dependencies externally:
# https://stackoverflow.com/a/27530173/15249616

# Use CMake to build, install and export a library:
# https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/
# https://cmake.org/cmake/help/git-stage/guide/importing-exporting/index.html
# Written based on this model:
# https://github.com/pabloariasal/modern-cmake-sample/blob/master/libjsonutils/CMakeLists.txt

# Cross-compilation guide : https://kubasejdak.com/how-to-cross-compile-for-embedded-with-cmake-like-a-champ
# https://dev.to/younup/cmake-on-stm32-episode-5-get-ready-for-c-1mh9
# https://github.com/modm-io/modm

message(VERBOSE "### HIVE ENTROPY API ###")

##############################################
# External project dependencies

##############################################
# Find system dependencies
find_package(OpenBLAS)
find_package(libcoap REQUIRED)

##############################################
# Create target and set properties

add_library(HiveEntropyAPI
        STATIC
        src/IncompatibleDimensionsException.cpp 
        src/OutOfBoundException.cpp
        src/Message.cpp
        src/CoapEndpoint.cpp
        src/HiveEntropyNode.cpp
        src/ResponseBuilder.cpp
        src/Hardware.cpp
        src/Peer.cpp
        src/Block.cpp
        src/Serializer.cpp
        src/Matrix.cpp src/Distributor.cpp)

# Add an alias so that library can be used inside the build tree, e.g. when testing
add_library(HiveEntropyAPI::HiveEntropyAPI ALIAS HiveEntropyAPI)

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

if(NOT TARGET OpenBLAS::OpenBLAS)
        message(STATUS "OpenBLAS exists but no OpenBLASTargets.cmake was found. Creating imported target...")
        add_library(OpenBLAS::OpenBLAS INTERFACE IMPORTED)
        if(OpenBLAS_LIBRARIES)
                set_target_properties(OpenBLAS::OpenBLAS PROPERTIES
                        INTERFACE_LINK_LIBRARIES "${OpenBLAS_LIBRARIES}"
                        INTERFACE_INCLUDE_DIRECTORIES "${OpenBLAS_INCLUDE_DIRS}"
                        )
        endif()
endif()

set(HIVEENTROPYAPI_COMPILE_OPTIONS -Wall -g)
target_compile_options(HiveEntropyAPI
        PRIVATE ${HIVEENTROPYAPI_COMPILE_OPTIONS}
)
message(VERBOSE "Compile options are ${HIVEENTROPYAPI_COMPILE_OPTIONS}")

# Link the OpenBlas library to the target
# See for static linking : https://stackoverflow.com/a/48966279/15249616
# Dependency should be made public ONLY if it is used in a public header
target_link_libraries(HiveEntropyAPI
        PUBLIC OpenBLAS::OpenBLAS libcoap::coap-3
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
export(EXPORT HiveEntropyAPITargets
        FILE "${CMAKE_CURRENT_BINARY_DIR}/HiveEntropyAPITargets.cmake"
        NAMESPACE HiveEntropyAPI::
)
message(STATUS "Exported HiveEntropyAPITargets.cmake to ${CMAKE_CURRENT_BINARY_DIR}")

message("")