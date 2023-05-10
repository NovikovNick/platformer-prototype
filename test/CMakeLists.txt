enable_testing(true)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure)

set(testFiles
    sync_test.cc
)

foreach(testFile ${testFiles})
    get_filename_component(testFilename "${testFile}" NAME_WLE)

    add_executable("${testFilename}" "${testFile}")
    target_link_libraries("${testFilename}" 
        PRIVATE  
            Platformer::core
            Platformer::model
            Platformer::converter)
    set_target_properties("${testFilename}" 
        PROPERTIES
		    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_test(NAME "${testFilename}" COMMAND "${testFilename}")

    add_dependencies(check "${testFilename}")
endforeach()