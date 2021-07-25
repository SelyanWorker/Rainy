#pragma once

#include "core/Time.h"
#include "core/Input.h"
#include "core/KeyCodes.h"
#include "core/Time.h"

#include "imgui.h"
#include "imgui_internal.h"

#include "../Terrain/Terrainn.h"
#include "../Terrain/Tools.h"
#include "../Terrain/BrushT.h"

constexpr float MAX_CAM_MOVE_SPEED = 10.f;
constexpr float MIN_CAM_MOVE_SPEED = 0.1f;

constexpr float MAX_CAM_ROTATE_SPEED = 45.0f;
constexpr float MIN_CAM_ROTATE_SPEED = 1.0f;

constexpr float MAX_BRUSH_SIZE = 10.f;
constexpr float MIN_BRUSH_SIZE = 0.1f;

constexpr float MAX_BRUSH_FALLOFF = 1.f;
constexpr float MIN_BRUSH_FALLOFF = 0.0f;

constexpr float MAX_SCULPT_STRENGTH = 3.f;
constexpr float MIN_SCULPT_STRENGTH = 0.1f;

constexpr float MAX_SMOOTH_STRENGTH = 0.5f;
constexpr float MIN_SMOOTH_STRENGTH = 0.01f;

constexpr uint32_t MAX_SMOOTH_LEVELS = 10;
constexpr uint32_t MIN_SMOOTH_LEVELS = 1;

extern te::ToolTypes toolType;
extern te::ToolManager* toolManager;
extern te::Terrain* terrain;
extern te::Brush* brush;

extern bool DRAW_TERRAIN_GRID;
extern bool DRAW_TERRAIN_NORMALS;

extern float CAM_MOVE_SPEED;
extern float CAM_ROTATE_SPEED;

extern float SCULPT_STRENGTH;
extern float SMOOTH_STRENGTH;
extern int SMOOTH_LEVELS;

extern bool b_ShowToolAndBrushMenu;
extern bool b_ShowGeneratorMenu;
extern bool b_ShowTerrainCreationMenu;
extern bool b_ShowUtilsMenu;

extern TerrainLayer* terrainLayer; // rewrite

class InterfaceLayer : public Rainy::Layer
{
public:
	InterfaceLayer()
	{
		SCULPT_STRENGTH = MIN_SCULPT_STRENGTH;
		SMOOTH_STRENGTH = MIN_SMOOTH_STRENGTH;
		SMOOTH_LEVELS = MIN_SMOOTH_LEVELS;
		toolType = te::ToolTypes::SCULPT;

	}

	~InterfaceLayer() = default;

	void OnEvent(Rainy::Event& e)
	{}

	void OnUpdate()
	{}

	void OnImGuiRender()
	{
		if (b_ShowToolAndBrushMenu) ShowToolAndBrushMenu();
		if (b_ShowGeneratorMenu) ShowGeneratorMenu();
		if (b_ShowTerrainCreationMenu) ShowTerrainCreationMenu();
		if (b_ShowUtilsMenu) ShowUtilsMenu();
		//ImGui::ShowDemoWindow();
	}

private:
	void ShowToolAndBrushMenu()
	{
		ImGui::Begin("Tool and Brush", &b_ShowToolAndBrushMenu, ImGuiWindowFlags_AlwaysAutoResize);
		{
			ImGui::LabelText("", "Tool");
			char const * toolNames[] = {
				"Sculpt", "Smooth", "Add section", "Delete section"
			};
			static int currentToolNumber = 0;
			ImGui::Combo("Type", &currentToolNumber, toolNames, IM_ARRAYSIZE(toolNames));
			{
				/*static float strength = MAX_STRENGTH;
				static int levels = 1;*/
				switch (currentToolNumber)
				{
				case 0:
					ImGui::SliderFloat("Strength", &SCULPT_STRENGTH,
						MIN_SCULPT_STRENGTH, MAX_SCULPT_STRENGTH);
					toolType = te::ToolTypes::SCULPT;
					break;
				case 1:
					ImGui::SliderFloat("Strength", &SMOOTH_STRENGTH,
						MIN_SMOOTH_STRENGTH, MAX_SMOOTH_STRENGTH);
					ImGui::SliderInt("Levels", &SMOOTH_LEVELS, MIN_SMOOTH_LEVELS, MAX_SMOOTH_LEVELS);
					toolType = te::ToolTypes::SMOOTH;
					break;
				case 2:
					toolType = te::ToolTypes::ADD_SECTION;
					break;
				case 3:
					toolType = te::ToolTypes::DELETE_SECTION;
					break;
				default:
					break;
				}
			}
		}

		if (toolType == te::ToolTypes::SCULPT || toolType == te::ToolTypes::SMOOTH)
		{
			ImGui::LabelText("", "Brush");
			{
				ImGui::SliderFloat("Size", &brush->size,
					MIN_BRUSH_SIZE, MAX_BRUSH_SIZE);
				ImGui::SliderFloat("Falloff", &brush->falloff,
					MIN_BRUSH_FALLOFF, MAX_BRUSH_FALLOFF);

				char const * falloffFunctions[] = {
					"No falloff", "Linear", "Smooth"
				};
				static int currentFalloffFun = 0;

				ImGui::Combo("Falloff type", &currentFalloffFun, falloffFunctions, IM_ARRAYSIZE(falloffFunctions));
				{
					switch (currentFalloffFun)
					{
					case 0:
						brush->SetFalloffFunction(te::Brush::BaseFalloff);
						break;
					case 1:
						brush->SetFalloffFunction(te::Brush::LinearFalloff);
						break;
					case 2:
						brush->SetFalloffFunction(te::Brush::SmoothFalloff);
						break;
					default:
						break;
					}
				}
			}
			//ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));
		}
		ImGui::End();
	}

	void ShowGeneratorMenu()
	{
		static std::string answer;

		static char imageDir[128] = "";
		static char imageName[32] = "";

		static int imageTypeNumber = 0;
		static int mapTypeNumber = 0;

		auto clear_fun = [&]() {
			for (uint16_t i = 0; i < 128 && imageDir[i]; ++i) imageDir[i] = 0;
			for (uint16_t i = 0; i < 32 && imageName[i]; ++i) imageName[i] = 0;
		};

		ImGui::Begin("Generator", &b_ShowGeneratorMenu, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::InputText("Image dir", imageDir, 128);
		ImGui::InputText("Image name", imageName, 32);


		ImGui::RadioButton("png", &imageTypeNumber, 0); // PNG
		ImGui::SameLine();
		ImGui::RadioButton("jpg", &imageTypeNumber, 1);  // JPEG

		ImGui::RadioButton("Height Map", &mapTypeNumber, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Normal Map", &mapTypeNumber, 1);

		if (ImGui::Button("Clear")) clear_fun();

		if (ImGui::Button("Generate"))
		{
			std::string path(imageDir);
			path += imageName;

			Rainy::Image* map = nullptr;
			Rainy::ImageTypes imageType = imageTypeNumber == 0 ? Rainy::ImageTypes::RN_PNG : Rainy::ImageTypes::RN_JPEG;
			if (imageType == Rainy::ImageTypes::RN_PNG) path += ".png";
			else path += ".jpeg";

			if (mapTypeNumber == 0)
			{
				map = terrain->GetHeightMapImage();
				map->SaveImage(path.c_str(), imageType);
			}
			else
			{
				map = terrain->GetNormalsMapImage();
				map->SaveImage(path.c_str(), imageType);
			}

			delete map;
		}

		ImGui::End();

		if (!b_ShowGeneratorMenu)
			answer.clear();
	}

	void ShowTerrainCreationMenu()
	{
		ImGui::Begin("New Terrain", &b_ShowTerrainCreationMenu, ImGuiWindowFlags_AlwaysAutoResize);

		constexpr size_t PATH_LENGTH = 128;
		static std::string heightMapPath; heightMapPath.resize(128);
		static std::string normalMapPath; normalMapPath.resize(128);
		static float heightMod = 1.f;
		ImGui::LabelText("", "Maps");
		ImGui::InputText("Height map path", heightMapPath.data(), PATH_LENGTH);
		//ImGui::SameLine();
		ImGui::InputScalar("Height mod", ImGuiDataType_Float, &heightMod, nullptr);
		//ImGui::InputText("Normal map path", normalMapPath.data(), PATH_LENGTH);

		ImGui::LabelText("", "Size");
		const char* items[] = { "2", "4", "8", "16", "32", "64", "128" };
		static int size0 = 0;
		static int size1 = 0;
		ImGui::Combo("Columns", &size0, items, IM_ARRAYSIZE(items));
		ImGui::SameLine();
		ImGui::Combo("Rows", &size1, items, IM_ARRAYSIZE(items));

		static float scale = 1.f;
		ImGui::SliderFloat("Quad size", &scale, 0.01f, 10.f);

		if (ImGui::Button("Create"))
		{
			Rainy::Image* heightMap = Rainy::Image::Create(heightMapPath.c_str());
			Rainy::Image* normalMap = Rainy::Image::Create(normalMapPath.c_str());

			te::TerrainInitSize columns = static_cast<te::TerrainInitSize>(std::stoi(items[size0]));
			te::TerrainInitSize rows = static_cast<te::TerrainInitSize>(std::stoi(items[size1]));

			brush->size *= terrain->GetScale(); // return brush size to normal

			delete terrain;
			delete toolManager;

			terrain = new te::Terrain(columns, rows, heightMap, normalMap, heightMod);
			toolManager = new te::ToolManager(terrain);

			terrain->SetScale(scale);
			brush->size /= terrain->GetScale();
		}
		ImGui::End();
	}

	void ShowUtilsMenu()
	{
		ImGui::Begin("Utils", &b_ShowUtilsMenu, ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::LabelText("", "Debug elements: ");
		ImGui::Checkbox("Draw terrain grid", &DRAW_TERRAIN_GRID);
		ImGui::Checkbox("Draw terrain normals", &DRAW_TERRAIN_NORMALS);

		ImGui::LabelText("", "Camera preferens: ");
		ImGui::SliderFloat("Move speed", &CAM_MOVE_SPEED,
			MIN_CAM_MOVE_SPEED, MAX_CAM_MOVE_SPEED);
		ImGui::SliderFloat("Rotate speed", &CAM_ROTATE_SPEED,
			MIN_CAM_ROTATE_SPEED, MAX_CAM_ROTATE_SPEED);

		ImGui::End();
	}

};
