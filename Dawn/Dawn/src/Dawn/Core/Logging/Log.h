#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Dawn 
{

	//
	// This class is taken from TheChernoProject Game Engine youtube series!
	//
	class DAWN_API Log
	{
	public: 
		static void Init();
		static void Flush();
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Core log macros
#define DWN_CORE_TRACE(...)    ::Dawn::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define DWN_CORE_INFO(...)     ::Dawn::Log::GetCoreLogger()->info(__VA_ARGS__)
#define DWN_CORE_WARN(...)     ::Dawn::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define DWN_CORE_ERROR(...)    ::Dawn::Log::GetCoreLogger()->error(__VA_ARGS__)
#define DWN_CORE_FATAL(...)    ::Dawn::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client log macros
#define DWN_TRACE(...)	      ::Dawn::Log::GetClientLogger()->trace(__VA_ARGS__)
#define DWN_INFO(...)	      ::Dawn::Log::GetClientLogger()->info(__VA_ARGS__)
#define DWN_WARN(...)	      ::Dawn::Log::GetClientLogger()->warn(__VA_ARGS__)
#define DWN_ERROR(...)	      ::Dawn::Log::GetClientLogger()->error(__VA_ARGS__)
#define DWN_FATAL(...)	      ::Dawn::Log::GetClientLogger()->fatal(__VA_ARGS__)
