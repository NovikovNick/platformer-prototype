add_executable (serialize "main.cc")
target_link_libraries(serialize 
    PRIVATE
        Platformer::core
)