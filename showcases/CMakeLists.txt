file(GLOB UI_SRC "ui/*.h" "ui/*.cc")

add_subdirectory (resources)

# SFML
set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 2.5 COMPONENTS graphics window main network audio REQUIRED)

add_executable (main_showcase "main_showcase.cc" ${UI_SRC})
target_compile_features(main_showcase PUBLIC cxx_std_20)
target_link_libraries(main_showcase 
    PRIVATE
        Platformer::core
        sfml-graphics
        sfml-window
        sfml-main
        sfml-network
        sfml-audio
)

add_executable (full_showcase "full_showcase.cc" ${UI_SRC})
target_compile_features(full_showcase PUBLIC cxx_std_20)
target_link_libraries(full_showcase 
    PRIVATE
        Platformer::core
        Platformer::serialization
        sfml-graphics
        sfml-window
        sfml-main
        sfml-network
        sfml-audio
)