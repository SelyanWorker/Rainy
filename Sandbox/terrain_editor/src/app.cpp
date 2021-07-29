#include "3d/box.h"
#include "application/entry_point.h"
#include "camera/orthographic_camera.h"
#include "camera/perspective_camera.h"
#include "imgui.h"
#include "light/light.h"
#include "render/frame_buffer.h"
#include "render/renderer.h"
#include "render/shader_library.h"

#include "Layers/InterfaceLayer.h"
#include "Layers/TerrainLayer.h"

TerrainLayer *terrainLayer = nullptr;
InterfaceLayer *interfaceLayer = nullptr;

bool b_ShowToolAndBrushMenu = false;
bool b_ShowGeneratorMenu = false;
bool b_ShowTerrainCreationMenu = false;
bool b_ShowUtilsMenu = false;

void Rainy::ImGuiLayer::MenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of other
            // windows, which we can't undo at the moment without finer window depth/z control.
            // ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

            if (ImGui::MenuItem("New", "", false))
            {
                b_ShowTerrainCreationMenu = !b_ShowTerrainCreationMenu;
            }

            /*if (ImGui::MenuItem("Save", "", false))
            {
                RN_APP_INFO("Save menu buttom");
            }

            if (ImGui::MenuItem("Save as ...", "", false))
            {
                RN_APP_INFO("");
            }*/

            /*ImGui::Separator();

            if (ImGui::MenuItem("Exit", "", false))
            {

            }*/

            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Tools and Brush", "", false))
        {
            b_ShowToolAndBrushMenu = !b_ShowToolAndBrushMenu;
        }

        if (ImGui::MenuItem("Generator", "", false))
        {
            b_ShowGeneratorMenu = !b_ShowGeneratorMenu;
        }

        if (ImGui::MenuItem("Utils", "", false))
        {
            b_ShowUtilsMenu = !b_ShowUtilsMenu;
        }

        /*if (ImGui::MenuItem("Help", "", false))
        {
            RN_APP_INFO("");
        }*/

        ImGui::EndMenuBar();
    }
}

class SandboxApp : public Rainy::Application
{
public:
    SandboxApp() : Application()
    {
        RN_APP_INFO("App run!");
        terrainLayer = new TerrainLayer();
        PushLayer(terrainLayer);

        interfaceLayer = new InterfaceLayer();
        PushLayer(interfaceLayer);
    }

    ~SandboxApp() {}
};

Rainy::Application *Rainy::CreateApplication() { return new SandboxApp(); }
