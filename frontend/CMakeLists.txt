﻿add_library (frontend_sync SHARED)
add_library (Platformer::frontend_sync ALIAS frontend_sync)
target_compile_features(frontend_sync PUBLIC cxx_std_20)
target_sources(frontend_sync 
	INTERFACE 
		"api.h"
	PRIVATE
		"sync/api.cc")
target_link_libraries(frontend_sync 
    PRIVATE
        Platformer::core
        Platformer::model
        Platformer::converter)
target_include_directories(frontend_sync 
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/.>
	PRIVATE 
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/.>)
set_property(TARGET frontend_sync PROPERTY
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")


add_library (frontend_async SHARED)
add_library (Platformer::frontend_async ALIAS frontend_async)
target_compile_features(frontend_async PUBLIC cxx_std_20)
target_sources(frontend_async 
	INTERFACE 
		"api.h"
	PRIVATE
		"async/api_async.cc"
		"async/core_game_loop.h"
		"async/core_game_loop.cc")
target_link_libraries(frontend_async 
    PRIVATE
        Platformer::core
        Platformer::model
        Platformer::converter
        winmm.lib)
target_include_directories(frontend_async 
	PUBLIC 
		${CMAKE_CURRENT_SOURCE_DIR})
set_property(TARGET frontend_async PROPERTY
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(GGPO_SRC
	"ggpo/api_ggpo.cc"
	"ggpo/input_args.h"
	"ggpo/net_game_loop.h"
	"ggpo/net_game_loop.cc"
	"ggpo/nongamestate.h")
	
set(NETWORK_SRC
	"ggpo/network/endpoint.h"
	"ggpo/network/exception.h"
	"ggpo/network/hex.h"
	"ggpo/network/stun_client.cc"
	"ggpo/network/stun_client.h"
	"ggpo/network/stun_protocol.h"
	"ggpo/network/udp_socket.h"
	"ggpo/network/udp_timeout_socket.cc")

add_library (frontend_ggpo SHARED)
add_library (Platformer::frontend_ggpo ALIAS frontend_ggpo)
target_compile_features(frontend_ggpo PUBLIC cxx_std_20)
target_sources(frontend_ggpo 
	INTERFACE 
		"api.h"
	PRIVATE
		${GGPO_SRC}
		${NETWORK_SRC})
target_link_libraries(frontend_ggpo 
    PRIVATE
        Platformer::core
        Platformer::model
        Platformer::converter
		ggpo::ggpo
        winmm.lib
		ws2_32.lib)
target_include_directories(frontend_ggpo 
	PUBLIC 
		${CMAKE_CURRENT_SOURCE_DIR})
set_property(TARGET frontend_ggpo PROPERTY
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")