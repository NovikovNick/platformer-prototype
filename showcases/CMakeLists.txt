file(GLOB UI_SRC "ui/*.h" "ui/*.cc")
file(GLOB APP_SRC "app/*.h" "app/*.cc")

add_subdirectory (resources)

# SFML
set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 2.5 COMPONENTS graphics window main REQUIRED)

add_executable (core_showcase "core_showcase.cc" ${UI_SRC})
target_compile_features(core_showcase PUBLIC cxx_std_20)
target_link_libraries(core_showcase 
    PRIVATE
        Platformer::core
        sfml-graphics
        sfml-window
        sfml-main
)

add_executable (serialization_showcase "serialization_showcase.cc" ${UI_SRC})
target_compile_features(serialization_showcase PUBLIC cxx_std_20)
target_link_libraries(serialization_showcase 
    PRIVATE
        Platformer::core
        Platformer::serialization
        sfml-graphics
        sfml-window
        sfml-main
)

add_executable (app_showcase "app_showcase.cc" ${UI_SRC} ${APP_SRC})
target_compile_features(app_showcase PUBLIC cxx_std_20)
target_link_libraries(app_showcase 
    PRIVATE
        Platformer::core
        Platformer::serialization
        ggpo::ggpo
        ${CMAKE_SOURCE_DIR}/external/ggpo/build/lib/x64/Debug/GGPO.lib
        sfml-graphics
        sfml-window
        sfml-main
)
add_custom_command(TARGET app_showcase POST_BUILD
	COMMAND  ${CMAKE_COMMAND} -E copy_if_different 
    ${CMAKE_SOURCE_DIR}/external/ggpo/build/bin/x64/Debug/GGPO.dll 
    ${CMAKE_CURRENT_BINARY_DIR}
)