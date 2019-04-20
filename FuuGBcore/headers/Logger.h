//
//  Logger.h
//  FuuGBcore
//
//  Created by Derek Boucher on 2019-04-20.
//

#ifndef Logger_h
#define Logger_h

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "Core.h"

namespace FuuGB
{
	class FUUGB_API Logger
	{
	public:
		static void init();
		static std::shared_ptr<spdlog::logger>& GetCpuLogger();
		static std::shared_ptr<spdlog::logger>& GetMemLogger();
		static std::shared_ptr<spdlog::logger>& GetPpuLogger();
		static std::shared_ptr<spdlog::logger>& GetSubsystemLogger();
	private:
		static std::shared_ptr<spdlog::logger> fuu_CpuLogger;
		static std::shared_ptr<spdlog::logger> fuu_MemLogger;
		static std::shared_ptr<spdlog::logger> fuu_PpuLogger;
		static std::shared_ptr<spdlog::logger> fuu_SubsystemLogger;
	};
}

#define FUUGB_CPU_LOG(...) FuuGB::Logger::GetCpuLogger()->warn(__VA_ARGS__)
#define FUUGB_MEM_LOG(...) FuuGB::Logger::GetMemLogger()->info(__VA_ARGS__)
#define FUUGB_PPU_LOG(...) FuuGB::Logger::GetPpuLogger()->error(__VA_ARGS__)
#define FUUGB_SUBSYSTEM_LOG(...) FuuGB::Logger::GetSubsystemLogger()->trace(__VA_ARGS__)

#endif Logger_h