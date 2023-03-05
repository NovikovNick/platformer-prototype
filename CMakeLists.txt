cmake_minimum_required (VERSION 3.24)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake")
project ("platformer")

# fpm
include_directories("external/fpm/include")

add_subdirectory ("core")
add_subdirectory ("showcases")
add_subdirectory ("serialization")