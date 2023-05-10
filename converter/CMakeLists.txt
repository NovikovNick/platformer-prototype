add_library(converter)
add_library (Platformer::converter ALIAS converter)
target_compile_features(converter PUBLIC cxx_std_20)

target_sources(converter 
	INTERFACE 
		"serializer.h"
	PRIVATE
		"serializer.cc")
target_link_libraries(converter 
	PRIVATE
		Platformer::core
		Platformer::model
)
target_include_directories(converter 
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	PRIVATE
		$<INSTALL_INTERFACE:./>
)
set_property(TARGET converter PROPERTY
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")