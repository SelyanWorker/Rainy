#pragma once

#include "application.h"
#include "core/log.h"

extern Rainy::Application *Rainy::createApplication();

//#ifdef RN_PLATFORM_WINDOWS
int main(int argc, char **argv)
{
    Rainy::Log::init();
    RN_CORE_INFO("Logger run!");

    auto app = Rainy::createApplication();

    app->run();

    return 0;
}
//#endif
