find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRC PROTO_HDR schema.proto)

add_library(model ${PROTO_HDR} ${PROTO_SRC})
add_library (Platformer::model ALIAS model)
target_compile_features(model PUBLIC cxx_std_17)
target_link_libraries(model 
	PUBLIC 
		${Protobuf_LIBRARIES} 
	PRIVATE 
		Platformer::core
)
target_include_directories(model 
	PUBLIC 
		${CMAKE_CURRENT_BINARY_DIR}
		${Protobuf_INCLUDE_DIRS}
)
set_property(TARGET model PROPERTY
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

