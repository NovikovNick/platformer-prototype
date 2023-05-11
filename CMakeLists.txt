cmake_minimum_required (VERSION 3.21)
project ("platformer"
    VERSION 0.1.1
    DESCRIPTION "Generic platformer library with rollback network"
    LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

enable_testing()
add_subdirectory ("external")
add_subdirectory ("core")
add_subdirectory ("model")
add_subdirectory ("converter")
add_subdirectory ("frontend")
add_subdirectory ("showcases")
add_subdirectory ("test")