file(GLOB UI_SRC "ui/*.h" "ui/*.cc")
file(GLOB APP_SRC "app/*.h" "app/*.cc")

add_subdirectory (resources)

# SFML
set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 2.5 COMPONENTS graphics window main REQUIRED)


add_executable (sync_showcase "sync_showcase.cc"
"ui/info.h"
"ui/info.cc"
"ui/rect_shape.h"
"ui/rect_shape.cc"
"ui/scalable_grid.h"
"ui/scalable_grid.cc")
target_compile_features(sync_showcase PUBLIC cxx_std_20)
target_link_libraries(sync_showcase 
    PRIVATE
        Platformer::core
        Platformer::model
        Platformer::converter
        Platformer::frontend_sync
        sfml-graphics
        sfml-window
        sfml-main)
set_target_properties(sync_showcase 
    PROPERTIES
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
        OUTPUT_NAME "${PROJECT_NAME}-sync-${CMAKE_PROJECT_VERSION}")


add_executable (async_showcase "async_showcase.cc"
"ui/info.h"
"ui/info.cc"
"ui/rect_shape.h"
"ui/rect_shape.cc"
"ui/scalable_grid.h"
"ui/scalable_grid.cc")
target_compile_features(async_showcase PUBLIC cxx_std_20)
target_link_libraries(async_showcase 
    PRIVATE
        Platformer::core
        Platformer::model
        Platformer::converter
        Platformer::frontend_async
        sfml-graphics
        sfml-window
        sfml-main)
set_target_properties(async_showcase 
    PROPERTIES
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
        OUTPUT_NAME "${PROJECT_NAME}-async-${CMAKE_PROJECT_VERSION}")


add_executable (ggpo_showcase "ggpo_showcase.cc"
"ui/info.h"
"ui/info.cc"
"ui/rect_shape.h"
"ui/rect_shape.cc"
"ui/scalable_grid.h"
"ui/scalable_grid.cc")
target_compile_features(ggpo_showcase PUBLIC cxx_std_20)
target_link_libraries(ggpo_showcase 
    PRIVATE
        Platformer::core
        Platformer::model
        Platformer::converter
        Platformer::frontend_ggpo
        sfml-graphics
        sfml-window
        sfml-main)
set_target_properties(ggpo_showcase 
    PROPERTIES
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
        OUTPUT_NAME "${PROJECT_NAME}-ggpo-${CMAKE_PROJECT_VERSION}")