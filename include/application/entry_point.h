#pragma once

#include "application.h"
#include "core/log.h"

extern Rainy::Application * Rainy::CreateApplication();

//#ifdef RN_PLATFORM_WINDOWS
int main(int argc, char** argv)
{
	Rainy::Log::Init();
	RN_CORE_INFO("Logger run!");

	auto app = Rainy::CreateApplication();

	app->Run();

	return 0;
}
//#endif
