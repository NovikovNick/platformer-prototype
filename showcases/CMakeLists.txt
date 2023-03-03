file(GLOB UI_SRC "ui/*.h" "ui/*.cc")

add_subdirectory (resources)

# SFML
set(SFML_DIR  "C:/home/app/SFML/lib/cmake/SFML")
set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 2.5 COMPONENTS graphics window main network audio REQUIRED)

add_executable (showcase "main.cc" ${UI_SRC})
target_link_libraries(showcase 
    PRIVATE
        sfml-graphics
        sfml-window
        sfml-main
        sfml-network
        sfml-audio
        Platformer::core
)