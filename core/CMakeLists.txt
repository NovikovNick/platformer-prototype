find_package(Eigen3 3.4 REQUIRED)

file(GLOB CORE_HDR "src/public/*.h")
file(GLOB CORE_SRC "src/private/*.cc")

add_library(core STATIC)
add_library (Platformer::core ALIAS core)
target_compile_features(core PUBLIC cxx_std_20)
target_sources(core 
	INTERFACE 
		${CORE_HDR} 
	PRIVATE 
		${CORE_SRC})
target_link_libraries(core 
	PUBLIC 
		fpm::fpm
		Eigen3::Eigen
		boost_ext::sml)
target_include_directories(core 
	PUBLIC 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/public>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/src/public>)
set_property(TARGET core PROPERTY
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")