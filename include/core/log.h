#pragma once

#include "core.h"

#include "spdlog/spdlog.h"

namespace Rainy
{
    class RAINY_API Log
    {
    public:
        static void init();

        inline static std::shared_ptr<spdlog::logger> &getCoreLogger() { return s_coreLogger; }

        inline static std::shared_ptr<spdlog::logger> &getAppLogger() { return s_appLogger; }

    private:
        static std::shared_ptr<spdlog::logger> s_coreLogger;
        static std::shared_ptr<spdlog::logger> s_appLogger;
    };
}

// engine log
#define RN_CORE_INFO(...) Rainy::Log::getCoreLogger()->info(__VA_ARGS__)
#define RN_CORE_TRACE(...) ::Rainy::Log::getCoreLogger()->trace(__VA_ARGS__)
#define RN_CORE_ERROR(...) ::Rainy::Log::getCoreLogger()->error(__VA_ARGS__)
#define RN_CORE_WARN(...) Rainy::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define RN_CORE_FATAL(...) Rainy::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// user log
#define RN_APP_INFO(...) Rainy::Log::getAppLogger()->info(__VA_ARGS__)
#define RN_APP_TRACE(...) Rainy::Log::getAppLogger()->trace(__VA_ARGS__)
#define RN_APP_ERROR(...) Rainy::Log::GetAppLogger()->error(__VA_ARGS__)
#define RN_APP_WARN(...) Rainy::Log::getAppLogger()->warn(__VA_ARGS__)
#define RN_APP_FATAL(...) Rainy::Log::GetAppLogger()->fatal(__VA_ARGS__)
