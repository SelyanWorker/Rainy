cmake_minimum_required(VERSION 3.7.2)

project(Rainy VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# vendors
add_subdirectory(vendor/glad)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/stb_image)

#imgui (will be removed in future)
set(imgui_path vendor/imgui)
file(GLOB imgui_src CONFIGURE_DEPENDS ${imgui_path}/*.cpp)
add_library(imgui
        STATIC
        ${imgui_src}
        ${imgui_path}/backends/imgui_impl_glfw.cpp
        ${imgui_path}/backends/imgui_impl_opengl3.cpp
        )
target_include_directories(imgui
        PUBLIC
        ${imgui_path}
        ${imgui_path}/backends
        )
target_link_libraries(imgui
        PUBLIC
        glfw
        glad
        PRIVATE
        Imm32
        )
add_executable(imgui_test ${imgui_path}/examples/example_glfw_opengl3/main.cpp)
target_link_libraries(imgui_test
        PRIVATE
        imgui
        )

file(GLOB rainy_src CONFIGURE_DEPENDS
        src/*.cpp
        platforms/OpenGL/*.cpp
        platforms/Windows/*.cpp
        platforms/stb_image/*.cpp
        )
message(STATUS ${rainy_src})
add_library(Rainy
        STATIC
        ${rainy_src}
        )
target_include_directories(Rainy
        PUBLIC
        include
        )
target_link_libraries(Rainy
        PUBLIC
        imgui
        spdlog::spdlog_header_only
        glfw
        glad
        stb_image
        )
target_precompile_headers(Rainy
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/pch.h
        )
#target_compile_definitions(Rainy PUBLIC RN_USING_DLL RN_BUILD_DLL)

add_subdirectory(Sandbox)

# platforms
#add_subdirectory(platforms/OpenGL)
#add_subdirectory(platforms/Windows)
#add_subdirectory(platforms/stb_image)
