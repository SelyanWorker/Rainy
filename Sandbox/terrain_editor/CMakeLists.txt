file(GLOB terrain_editor_src
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Layers/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Terrain/*.cpp
        )
add_library(terrain_editor_lib
        STATIC
        ${terrain_editor_src}
        )
target_link_libraries(terrain_editor_lib
        PUBLIC
        Rainy
        )

add_executable(terrain_editor ${CMAKE_CURRENT_SOURCE_DIR}/src/app.cpp)
target_link_libraries(terrain_editor
        PRIVATE
        terrain_editor_lib
        )
