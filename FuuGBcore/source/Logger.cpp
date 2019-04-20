//
//  Logger.cpp
//  FuuGBcore
//
//  Created by Derek Boucher on 2019-04-20.
//

#include "Fuupch.h"
#include "Logger.h"

namespace FuuGB
{
	std::shared_ptr<spdlog::logger> Logger::fuu_CpuLogger;
	std::shared_ptr<spdlog::logger> Logger::fuu_MemLogger;
	std::shared_ptr<spdlog::logger> Logger::fuu_PpuLogger;
	std::shared_ptr<spdlog::logger> Logger::fuu_SubsystemLogger;

	void Logger::init()
	{
		fuu_CpuLogger = spdlog::stdout_color_mt("CPU");
		fuu_MemLogger = spdlog::stdout_color_mt("MEM");
		fuu_PpuLogger = spdlog::stdout_color_mt("PPU");
		fuu_SubsystemLogger = spdlog::stdout_color_mt("SUBSYSTEM");

		fuu_CpuLogger->set_pattern("%^[%T] %n: %v%$");
		fuu_CpuLogger->set_level(spdlog::level::trace);

		fuu_MemLogger->set_pattern("%^[%T] %n: %v%$");
		fuu_MemLogger->set_level(spdlog::level::trace);

		fuu_PpuLogger->set_pattern("%^[%T] %n: %v%$");
		fuu_PpuLogger->set_level(spdlog::level::trace);

		fuu_SubsystemLogger->set_pattern("%^[%T] %n: %v%$");
		fuu_SubsystemLogger->set_level(spdlog::level::trace);
	}

	std::shared_ptr<spdlog::logger>& Logger::GetCpuLogger()
	{
		return Logger::fuu_CpuLogger;
	}
	std::shared_ptr<spdlog::logger>& Logger::GetMemLogger()
	{
		return Logger::fuu_MemLogger;
	}
	std::shared_ptr<spdlog::logger>& Logger::GetPpuLogger()
	{
		return Logger::fuu_PpuLogger;
	}
	std::shared_ptr<spdlog::logger>& Logger::GetSubsystemLogger()
	{
		return Logger::fuu_SubsystemLogger;
	}
}
