cmake_minimum_required (VERSION 3.24)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake")
project ("platformer")

add_subdirectory ("external")
add_subdirectory ("core")
add_subdirectory ("serialization")
add_subdirectory ("frontend")
add_subdirectory ("showcases")