#include "application/Application.h"
#include "application/EntryPoint.h"

class SandboxApp : public Rainy::Application
{
public:

	SandboxApp()
	:	Application()
	{
		RN_APP_INFO("App run!");
	}

	~SandboxApp()
	{
	}

};

Rainy::Application* Rainy::CreateApplication()
{
	return new SandboxApp();
}

