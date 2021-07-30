file(GLOB terrain_editor_src
        CONFIGURE_DEPENDS
        terrain_editor/layers/src/*.cpp
        terrain_editor/terrain/src/*.cpp
        terrain_editor/tools/src/*.cpp
        terrain_editor/globals.cpp
        )
add_library(terrain_editor_lib
        STATIC
        ${terrain_editor_src}
        )
target_link_libraries(terrain_editor_lib
        PUBLIC
        Rainy
        )

add_executable(terrain_editor terrain_editor/app.cpp)
target_link_libraries(terrain_editor
        PRIVATE
        terrain_editor_lib
        )
