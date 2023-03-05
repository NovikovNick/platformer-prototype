find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRC PROTO_HDR message.proto)
add_library(serialization ${PROTO_SRC} ${PROTO_HDR})
add_library (Platformer::serialization ALIAS serialization)
target_link_libraries(serialization 
	PUBLIC 
		protobuf::libprotoc 
		protobuf::libprotobuf 
		protobuf::libprotobuf-lite)
target_include_directories(serialization 
	PUBLIC
	  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
	  $<INSTALL_INTERFACE:./>
)
