file(GLOB CORE_HEADERS "src/*.h")
file(GLOB CORE_SRC "src/*.cc")

add_library(core STATIC)
target_sources(core INTERFACE ${CORE_HEADERS} PRIVATE ${CORE_SRC})
target_include_directories(core PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/src>
)
add_library (Platformer::core ALIAS core)