find_package(Eigen3 3.4 REQUIRED)

file(GLOB CORE_HDR "src/*.h")
file(GLOB CORE_SRC "src/*.cc")

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
		Eigen3::Eigen)
target_include_directories(core 
	PUBLIC 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/src>)