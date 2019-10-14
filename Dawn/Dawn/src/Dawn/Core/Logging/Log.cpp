#include "Log.h"
#include "spdlog/sinks/msvc_sink.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "inc_core.h"

namespace Dawn 
{
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init() 
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
		auto filesink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("Log.txt", true);
		s_CoreLogger = Shared<spdlog::logger>(new spdlog::logger("DAWN", { sink, filesink }));
		s_CoreLogger->set_level(spdlog::level::trace); 
		
		auto sink2 = std::make_shared<spdlog::sinks::msvc_sink_mt>();
		s_ClientLogger = Shared<spdlog::logger>(new spdlog::logger("APP", { sink2, filesink }));
		s_ClientLogger->set_level(spdlog::level::trace);

	}

	void Log::Flush()
	{
		s_CoreLogger->flush();
		s_ClientLogger->flush();
	}

}