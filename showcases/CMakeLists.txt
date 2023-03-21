file(GLOB UI_SRC "ui/*.h" "ui/*.cc")
file(GLOB APP_SRC "app/*.h" "app/*.cc")

add_subdirectory (resources)

# SFML
set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 2.5 COMPONENTS graphics window main REQUIRED)

add_executable (core_showcase "core_showcase.cc" ${UI_SRC} "app/core_game_loop.h" "app/core_game_loop.cc")
target_compile_features(core_showcase PUBLIC cxx_std_20)
target_link_libraries(core_showcase 
    PRIVATE
        Platformer::core
        winmm.lib
        sfml-graphics
        sfml-window
        sfml-main
)

add_executable (ggpo_showcase "ggpo_showcase.cc" ${UI_SRC} ${APP_SRC})
target_compile_features(ggpo_showcase PUBLIC cxx_std_20)
target_link_libraries(ggpo_showcase 
    PRIVATE
        Platformer::core
        Platformer::serialization
        sfml-graphics
        sfml-window
        sfml-main
)
target_link_libraries(ggpo_showcase 
    LINK_PUBLIC 
        ggpo::ggpo
        winmm.lib
        ws2_32.lib
        ${CMAKE_SOURCE_DIR}/external/ggpo/build/lib/x64/Debug/GGPO.lib
)
add_custom_command(TARGET ggpo_showcase POST_BUILD
	COMMAND  ${CMAKE_COMMAND} -E copy_if_different 
    ${CMAKE_SOURCE_DIR}/external/ggpo/build/bin/x64/${CMAKE_BUILD_TYPE}/GGPO.dll 
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable (frontend_sync_showcase "frontend_sync_showcase.cc"
"ui/info.h"
"ui/info.cc"
"ui/rect_shape.h"
"ui/rect_shape.cc"
"ui/scalable_grid.h"
"ui/scalable_grid.cc")
target_compile_features(frontend_sync_showcase PUBLIC cxx_std_20)
target_link_libraries(frontend_sync_showcase 
    PRIVATE
        Platformer::core
        Platformer::serialization
        Platformer::frontend_sync
        winmm.lib
        sfml-graphics
        sfml-window
        sfml-main
)