set(CMAKE_CXX_STANDARD 20)

add_subdirectory (resources)

file(GLOB MATH_SRC "./src/*.h" "./src/*.cc")

# Eigen 3.4
include_directories("../external/eigen")

# fpm
include_directories("../external/fpm/include")

# SFML
set(SFML_DIR  "C:/home/app/SFML/lib/cmake/SFML")
set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 2.5 COMPONENTS graphics window main network audio REQUIRED)

add_executable (math "main.cc" ${MATH_SRC})
target_link_libraries(math 
PRIVATE
    sfml-graphics
    sfml-window
    sfml-main
    sfml-network
    sfml-audio
)
