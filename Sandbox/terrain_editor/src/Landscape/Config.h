#pragma once

#include <cstdint>


constexpr uint8_t GL_VERSION_MAJOR = 4;
constexpr uint8_t GL_VERSION_MINOR = 0;
const char * const GLSL_VERSION = "#version 400";

constexpr float
	RED_COLOR_CLEAR = 0.9f + 0.1f,
	GREEN_COLOR_CLEAR = 0.9f + 0.1f,
	BLUE_COLOR_CLEAR = 0.9f + 0.1f,
	ALPHA_CLEAR = 1.f;

constexpr float LINE_WIDTH = 10.f;

constexpr uint16_t WIDTH = 1024;
constexpr uint16_t HEIGHT = 768;
char const * const TITLE = "Landscape Editor";

constexpr float FAR_PLANE = 1000.f;
constexpr float NEAR_PLANE = 0.1f;
constexpr float FOV = 60.f;

constexpr float MOVE_SPEED = 0.03f;
constexpr float ROT_SPEED = 0.2f;

constexpr float RAY_DISTANCE = 10000.f;

constexpr float TERRAIN_SECTION_SIZE = 1.f;

constexpr uint32_t MIN_LANDSCAPE_SIZE = 10;
constexpr uint32_t MAX_LANDSCAPE_SIZE = 1000;

constexpr float MIN_LANDSCAPE_ELEMENT_SIZE = 0.01f;
constexpr float OPTIMAL_LANDSCAPE_ELEMENT_SIZE = 0.1f;
constexpr float MAX_LANDSCAPE_ELEMENT_SIZE = 1;

constexpr float MIN_SCULPT_STRENGTH = 0.01f;
constexpr float MAX_SCULPT_STRENGTH = 0.10f;

constexpr float MIN_FALLOFF = 0.f;
constexpr float STANDART_FALLOFF = 0.5f;
constexpr float MAX_FALLOFF = 1.f;
constexpr float MIN_BRUSH_SIZE = MIN_LANDSCAPE_ELEMENT_SIZE;
constexpr float OPTIMAL_BRUSH_SIZE = 1.f;
constexpr float MAX_BRUSH_SIZE = 3.f;
	
constexpr float MIN_STRENGTH_MOD = 0.25f;

constexpr float MAX_HEIGHT = 2.5f;
