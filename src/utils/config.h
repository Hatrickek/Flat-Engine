#pragma once
#include <filesystem>
#include <string>

namespace FlatEngine{
    class Config
    {
    public:
        static void LoadInternalConfig();
        static void SaveInternalConfig();
        static bool LoadConfig();
        static void LoadConfig(std::filesystem::path path);
        static void SaveConfig();
    };
}
