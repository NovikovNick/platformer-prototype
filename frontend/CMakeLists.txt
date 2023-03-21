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
        Platformer::serialization)
target_include_directories(frontend_sync 
	PUBLIC 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/.>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/.>)