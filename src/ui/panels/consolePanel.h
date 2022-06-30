#include "spdlog/common.h"
namespace FlatEngine{
    class ConsolePanel{
    public:
        static void DrawConsole();
        static void AddLog(const char* fmt, spdlog::level::level_enum level);
    private:
    };
}