add_library (frontend_sync SHARED)
add_library (Platformer::frontend_sync ALIAS frontend_sync)
target_compile_features(frontend_sync PUBLIC cxx_std_20)
target_sources(frontend_sync 
	INTERFACE 
		"api.h"
	PRIVATE
		"impl/api.cc")
target_link_libraries(frontend_sync 
    PRIVATE
        Platformer::core
        Platformer::model
        Platformer::converter
)
target_include_directories(frontend_sync 
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/.>
	PRIVATE 
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/.>)

set_property(TARGET frontend_sync PROPERTY
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")