#pragma once

#include "core.h"

#include "spdlog/spdlog.h"

namespace Rainy {
	class RAINY_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_coreLogger; }

		inline static std::shared_ptr<spdlog::logger>& GetAppLogger() { return s_appLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_coreLogger;
		static std::shared_ptr<spdlog::logger> s_appLogger;
	};
}

// engine log
#define RN_CORE_INFO(...) Rainy::Log::GetCoreLogger()->info(__VA_ARGS__)
#define RN_CORE_TRACE(...) ::Rainy::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define RN_CORE_ERROR(...) ::Rainy::Log::GetCoreLogger()->error(__VA_ARGS__)
#define RN_CORE_WARN(...)	Rainy::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define RN_CORE_FATAL(...) Rainy::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// user log
#define RN_APP_INFO(...) Rainy::Log::GetAppLogger()->info(__VA_ARGS__)
#define RN_APP_TRACE(...) Rainy::Log::GetAppLogger()->trace(__VA_ARGS__)
#define RN_APP_ERROR(...) Rainy::Log::GetAppLogger()->error(__VA_ARGS__)
#define RN_APP_WARN(...) Rainy::Log::GetAppLogger()->warn(__VA_ARGS__)
#define RN_APP_FATAL(...) Rainy::Log::GetAppLogger()->fatal(__VA_ARGS__)
