add_library(ggpo STATIC IMPORTED GLOBAL)
add_library (ggpo::ggpo ALIAS ggpo)
target_include_directories(ggpo INTERFACE ggpo/src/include)
set_target_properties(ggpo 
    PROPERTIES 
        LINKER_LANGUAGE CXX
        IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/ggpo/build/lib/x64/${CMAKE_BUILD_TYPE}/GGPO.lib)

add_library(fpm INTERFACE)
add_library (fpm::fpm ALIAS fpm)
target_include_directories(fpm INTERFACE fpm/include)
install(FILES
  fpm/include/fpm/fixed.hpp
  fpm/include/fpm/ios.hpp
  fpm/include/fpm/math.hpp
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/fpm)

add_library(sml INTERFACE)
add_library (boost_ext::sml ALIAS sml)
target_include_directories(sml INTERFACE boost)
install(FILES
  boost/sml.hpp
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/boost)