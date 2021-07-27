#include "core/log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Rainy {

	std::shared_ptr<spdlog::logger> Log::s_coreLogger;
	std::shared_ptr<spdlog::logger> Log::s_appLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		s_coreLogger = spdlog::stdout_color_mt("CoreLogger");
		s_coreLogger->set_level(spdlog::level::trace);
		s_appLogger = spdlog::stdout_color_mt("AppLogger");
		s_appLogger->set_level(spdlog::level::trace);
	}
}
