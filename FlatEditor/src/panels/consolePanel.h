#pragma once
#include "spdlog/common.h"
namespace FlatEngine {
	class ConsolePanel {
	public:
		static void DrawPanel();
		static void AddLog(const char* fmt, spdlog::level::level_enum level);
	};
}