find_package(Protobuf REQUIRED)
set(DLL_EXPORT "__declspec(dllexport)")
set(Protobuf_DEBUG true)
protobuf_generate_cpp(PROTO_SRC PROTO_HDR EXPORT_MACRO ${DLL_EXPORT} schema.proto)

add_library(model SHARED)
add_library (Platformer::model ALIAS model)
target_compile_features(model PUBLIC cxx_std_20)

target_sources(model 
	PUBLIC 
		${PROTO_HDR}
		${PROTO_SRC})
target_link_libraries(model 
	PUBLIC 
		${Protobuf_LIBRARIES}
	PRIVATE
		Platformer::core
)
target_include_directories(model 
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
		${Protobuf_INCLUDE_DIRS}
	PRIVATE
		$<INSTALL_INTERFACE:./>
)

