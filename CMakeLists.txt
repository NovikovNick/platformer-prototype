cmake_minimum_required (VERSION 3.13)

project ("platformer")

set(CMAKE_CXX_STANDARD 20) # todo: use targets instead 

# Eigen 3.4
include_directories("external/eigen")

# fpm
include_directories("external/fpm/include")

add_subdirectory ("core")
add_subdirectory ("showcases")
add_subdirectory ("api")