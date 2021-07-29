#pragma once

#include <memory>
#include <sstream>
#include <string>

const std::string TERRAIN_SHADER_SRC =
#include "Terrain.glsl"
    ;

const std::string DRAW_NORMALS_SHADER_SRC =
#include "DrawTerrainNormals.glsl"
    ;

const std::string POINTS_SHADER_SRC =
#include "Points.glsl"
    ;

const std::string BASE_SHADER_SRC =
#include "Base.glsl"
    ;

std::unique_ptr<std::istream> TERRAIN_SHADER_SRC_STREAM_PTR =
    std::make_unique<std::istringstream>(TERRAIN_SHADER_SRC);
std::unique_ptr<std::istream> DRAW_NORMALS_SHADER_PTR =
    std::make_unique<std::istringstream>(DRAW_NORMALS_SHADER_SRC);
std::unique_ptr<std::istream> POINTS_SHADER_PTR =
    std::make_unique<std::istringstream>(POINTS_SHADER_SRC);
std::unique_ptr<std::istream> BASE_SHADER_PTR =
    std::make_unique<std::istringstream>(BASE_SHADER_SRC);
