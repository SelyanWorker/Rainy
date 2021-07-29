#include "application/application.h"
#include "application/entry_point.h"

void Rainy::ImGuiLayer::menuBar() {}

class SandboxApp : public Rainy::Application
{
public:
    SandboxApp() : Application() { RN_APP_INFO("App run!"); }

    ~SandboxApp() {}
};

Rainy::Application *Rainy::createApplication() { return new SandboxApp(); }
