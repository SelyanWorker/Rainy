#include "../res/shaders/Shaders.h"

#include "tools/include/tools.h"

te::ToolTypes toolType = te::ToolTypes::NONE;
te::ToolManager *toolManager = nullptr;
te::Terrain *terrain = nullptr;
te::Brush *brush = nullptr;

bool DRAW_TERRAIN_GRID = false;
bool DRAW_TERRAIN_NORMALS = false;

float CAM_MOVE_SPEED = 2.0f;
float CAM_ROTATE_SPEED = 15.0f;

float SCULPT_STRENGTH = 1.f;
float SMOOTH_STRENGTH = 0.5f;
int SMOOTH_LEVELS = 1;
