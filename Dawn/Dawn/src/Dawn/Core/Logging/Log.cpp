#include "Log.h"
#include "spdlog/sinks/msvc_sink.h"

namespace Dawn 
{
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init() 
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
		s_CoreLogger = std::make_shared<spdlog::logger>("DAWN", sink);
		s_CoreLogger->set_level(spdlog::level::trace); 
		
		auto sink2 = std::make_shared<spdlog::sinks::msvc_sink_mt>();
		s_ClientLogger = std::make_shared<spdlog::logger>("APP", sink2);
		s_ClientLogger->set_level(spdlog::level::trace);
	}

}