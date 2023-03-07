find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRC PROTO_HDR schema.proto)
add_library(serialization STATIC)
add_library (Platformer::serialization ALIAS serialization)

target_sources(serialization 
	INTERFACE 
		"serializer.h"
	PRIVATE
		"serializer.cc"
		${PROTO_SRC} 
		${PROTO_HDR})
target_link_libraries(serialization 
	PRIVATE 
		Platformer::core
		protobuf::libprotoc 
		protobuf::libprotobuf 
		protobuf::libprotobuf-lite
)

target_include_directories(serialization 
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	PRIVATE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
		$<INSTALL_INTERFACE:./>
)
