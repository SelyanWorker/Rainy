#include "../include/application/application.h"

#include "core/input.h"
#include "core/window.h"
#include "imgui_layer/imgui_layer.h"
#include "render/renderer.h"

#include "GLFW/glfw3.h"

namespace Rainy
{
    uint8_t KeyStates[512];

    Application *Application::m_application = nullptr;

    ImGuiLayer *imguilayer;

    Application::Application()
    {
        m_application = this;
        m_frameTime = 0;
        m_run = true;

        m_window = Window::Create({ "Terrain Editor", 1024, 768 });
        RN_APP_INFO("GLFW windows create!");

        m_window->SetEventFunction(RN_BIND_MEMBER_FUN(Application::OnEvent));

        for (uint16_t i = 0; i < 512; i++)
            KeyStates[i] = 0;

        imguilayer = new ImGuiLayer();
        PushLayer(imguilayer);
    }

    Application::~Application() { ShutDown(); }

    void Application::PushLayer(Layer *layer) { m_layerStack.PushLayer(layer); }

    void Application::PopLayer() { m_layerStack.PopLayer(); }

    Application *Application::Get() { return m_application; }

    Window *Application::GetWindow() { return m_window; }

    float Application::GetFrameTime() const { return m_frameTime; }

    void Application::Run()
    {
        Setup();

        constexpr float BORDER_FRAME_TIME = 1.f / 120.f;
        TimeStep time = TimeStep::GetTime();
        for (; m_run;)
        {
            m_window->OnUpdate();

            Clear();

            for (auto layer : m_layerStack)
            {
                layer->OnUpdate();
            }

            imguilayer->Begin();
            for (auto layer : m_layerStack)
            {
                layer->OnImGuiRender();
            }
            imguilayer->End();

            m_window->SwapBuffers();

            TimeStep currentTime = TimeStep::GetTime();
            m_frameTime = currentTime.GetSeconds() - time.GetSeconds();
            for (; m_frameTime <= BORDER_FRAME_TIME;)
            {
                currentTime = TimeStep::GetTime();
                m_frameTime = currentTime.GetSeconds() - time.GetSeconds();
            }
            time = currentTime;
        }

        ShutDown();
    }

    void Application::EnableCursor() { m_window->EnableCursor(); }

    void Application::DisableCursor() { m_window->DisableCursor(); }

    void Application::OnEvent(Event &e)
    {
        EventDispetcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(RN_BIND_MEMBER_FUN(Application::OnWindowClose));

        for (auto layer : m_layerStack)
        {
            layer->OnEvent(e);
            if (e.Handled)
                break;
        }
    }

    bool Application::OnWindowClose(WindowCloseEvent &e)
    {
        m_run = false;
        return true;
    }

    /*bool Application::OnWindowResize(WindowResizeEvent& e)
    {
        return false;
    }*/

    /*bool Application::OnKeyPressed(KeyPressedEvent& e)
    {
        if(!KeyStates[e.GetKeyCode()]) RN_APP_TRACE("Key pressed {0}", e.GetKeyCode());
        KeyStates[e.GetKeyCode()] = 1;
    }
    bool Application::OnKeyRelease(KeyReleaseEvent& e)
    {
        if (KeyStates[e.GetKeyCode()]) RN_APP_TRACE("Key release {0}", e.GetKeyCode());
        KeyStates[e.GetKeyCode()] = 0;
    }*/

    void Application::ShutDown()
    {
        delete m_window;
        RN_APP_INFO("Application shutdown.");
    }
}
