file(GLOB UI_SRC "ui/*.h" "ui/*.cc")
file(GLOB APP_SRC "app/*.h" "app/*.cc")

add_subdirectory (resources)

# SFML
set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 2.5 COMPONENTS graphics window main REQUIRED)

add_executable (ggpo_showcase "ggpo_showcase.cc" ${UI_SRC} ${APP_SRC})
target_compile_features(ggpo_showcase PUBLIC cxx_std_20)
target_link_libraries(ggpo_showcase 
    PRIVATE
        Platformer::core
        Platformer::model
        Platformer::converter
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
        winmm.lib
        sfml-graphics
        sfml-window
        sfml-main
)
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
        winmm.lib
        sfml-graphics
        sfml-window
        sfml-main
)
set_target_properties(async_showcase 
    PROPERTIES
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
        OUTPUT_NAME "${PROJECT_NAME}-async-${CMAKE_PROJECT_VERSION}")