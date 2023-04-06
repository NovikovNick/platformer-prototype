cmake_minimum_required (VERSION 3.15)
set(VCPKG_TARGET_TRIPLET x64-windows-static)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake")
project ("platformer"
    VERSION 0.0.4
    DESCRIPTION "Generic platformer library with rollback network"
    LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

add_subdirectory ("external")
add_subdirectory ("core")
add_subdirectory ("model")
add_subdirectory ("converter")
add_subdirectory ("frontend")
add_subdirectory ("showcases")