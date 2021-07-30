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

    ImGuiLayer *imguiLayer;

    Application::Application()
    {
        m_application = this;
        m_frameTime = 0;
        m_run = true;

        m_window = Window::create({ "Terrain Editor", 1024, 768 });
        RN_APP_INFO("GLFW windows create!");

        m_window->setEventFunction(RN_BIND_MEMBER_FUN(Application::onEvent));

        for (uint16_t i = 0; i < 512; i++)
            KeyStates[i] = 0;

        imguiLayer = new ImGuiLayer();
        pushLayer(imguiLayer);
    }

    Application::~Application() { shutDown(); }

    void Application::pushLayer(Layer *layer) { m_layerStack.pushLayer(layer); }

    void Application::popLayer() { m_layerStack.popLayer(); }

    Application *Application::get() { return m_application; }

    Window *Application::getWindow() { return m_window; }

    float Application::getFrameTime() const { return m_frameTime; }

    void Application::run()
    {
        Setup();

        constexpr float BORDER_FRAME_TIME = 1.f / 120.f;
        TimeStep time = TimeStep::getTime();
        for (; m_run;)
        {
            m_window->onUpdate();

            Clear();

            for (auto layer : m_layerStack)
            {
                layer->onUpdate();
            }

            imguiLayer->begin();
            for (auto layer : m_layerStack)
            {
                layer->onImGuiRender();
            }
            imguiLayer->end();

            m_window->swapBuffers();

            TimeStep currentTime = TimeStep::getTime();
            m_frameTime = currentTime.getSeconds() - time.getSeconds();
            for (; m_frameTime <= BORDER_FRAME_TIME;)
            {
                currentTime = TimeStep::getTime();
                m_frameTime = currentTime.getSeconds() - time.getSeconds();
            }
            time = currentTime;
        }

        shutDown();
    }

    void Application::enableCursor() { m_window->enableCursor(); }

    void Application::disableCursor() { m_window->disableCursor(); }

    void Application::onEvent(Event &e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.dispatch<WindowCloseEvent>(RN_BIND_MEMBER_FUN(Application::onWindowClose));

        for (auto layer : m_layerStack)
        {
            layer->onEvent(e);
            if (e.Handled)
                break;
        }
    }

    bool Application::onWindowClose(WindowCloseEvent &e)
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
        if (KeyStates[e.GetKeyCode()]) RN_APP_TRACE("Key release {0}", e.getKeyCode());
        KeyStates[e.getKeyCode()] = 0;
    }*/

    void Application::shutDown()
    {
        delete m_window;
        RN_APP_INFO("Application shutdown.");
    }
}
