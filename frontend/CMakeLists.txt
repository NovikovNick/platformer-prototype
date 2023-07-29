add_library (frontend_sync SHARED)
add_library (Platformer::frontend_sync ALIAS frontend_sync)
target_compile_features(frontend_sync PUBLIC cxx_std_20)
target_sources(frontend_sync 
	INTERFACE 
		"src/public/api.h"
	PRIVATE
		"src/private/sync/api.cc")
target_link_libraries(frontend_sync 
    PRIVATE
        Platformer::core
        Platformer::model
        Platformer::converter)
target_include_directories(frontend_sync 
	PUBLIC 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/public>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/src/public>)
set_property(TARGET frontend_sync PROPERTY
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")


add_library (frontend_async SHARED)
add_library (Platformer::frontend_async ALIAS frontend_async)
target_compile_features(frontend_async PUBLIC cxx_std_20)
target_sources(frontend_async 
	INTERFACE 
		"src/public/api.h"
	PRIVATE
		"src/public/game_loop_ticker.h"
		"src/private/async/api_async.cc"
		"src/private/game_loop_ticker.cc")
target_link_libraries(frontend_async 
    PRIVATE
        Platformer::core
        Platformer::model
        Platformer::converter
        winmm.lib)
target_include_directories(frontend_async 
	PUBLIC 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/public>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/src/public>)
set_property(TARGET frontend_async PROPERTY
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(GGPO_SRC
	"src/public/ggpo/input_args.h"
	"src/public/ggpo/nongamestate.h"
	"src/public/ggpo/ggpo_client.h"
	"src/public/game_loop_ticker.h"
	"src/private/ggpo/api_ggpo.cc"
	"src/private/game_loop_ticker.cc")
	
set(NETWORK_SRC
	"src/public/ggpo/network/endpoint.h"
	"src/public/ggpo/network/exception.h"
	"src/public/ggpo/network/hex.h"
	"src/public/ggpo/network/stun_client.h"
	"src/public/ggpo/network/stun_protocol.h"
	"src/public/ggpo/network/udp_socket.h"
	"src/private/ggpo/network/stun_client.cc"
	"src/private/ggpo/network/udp_timeout_socket.cc")

add_library (frontend_ggpo SHARED)
add_library (Platformer::frontend_ggpo ALIAS frontend_ggpo)
target_compile_features(frontend_ggpo PUBLIC cxx_std_20)
target_sources(frontend_ggpo 
	INTERFACE 
		"src/public/api.h"
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
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/public>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/src/public>)
set_property(TARGET frontend_ggpo PROPERTY
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")