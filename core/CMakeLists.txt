set(CMAKE_CXX_STANDARD 20)

add_subdirectory (resources)

file(GLOB MATH_SRC "./src/*.h" "./src/*.cc")

# Eigen 3.4
include_directories("../external/eigen")

# fpm
include_directories("../external/fpm/include")

# SFML for x64 with BUILD_SHARED_LIBS=false, SFML_USE_STATIC_STD_LIBS=false
set(SFML_DIR "../../SFML-2.5.1/SFML_build")
set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML COMPONENTS graphics window system audio REQUIRED)

add_executable (math "main.cc" ${MATH_SRC})
target_link_libraries(math sfml-graphics sfml-window sfml-system sfml-network sfml-audio opengl32 winmm gdi32 )
