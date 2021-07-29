#include "application/application.h"
#include "application/entry_point.h"

void Rainy::ImGuiLayer::MenuBar() {}

class SandboxApp : public Rainy::Application
{
public:
    SandboxApp() : Application() { RN_APP_INFO("App run!"); }

    ~SandboxApp() {}
};

Rainy::Application *Rainy::CreateApplication() { return new SandboxApp(); }
