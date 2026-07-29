#pragma once

#include <filesystem>

namespace cnt
{
    inline std::filesystem::path getAssetsBasePath()
    {
        static std::filesystem::path base = []()
        {
            std::filesystem::path current = std::filesystem::current_path();
            for (size_t i = 0; i < 4; ++i)
            {
                if (std::filesystem::exists(current / "client" / "assets"))
                {
                    return current / "client" / "assets";
                }
                if (std::filesystem::exists(current / "assets"))
                {
                    return current / "assets";
                }
                current = current.parent_path();
            }
            throw std::runtime_error("Can't find asssets directory! Current directory: " + std::filesystem::current_path().string());
        }();
        return base;
    }

    inline std::filesystem::path logs = getAssetsBasePath() / "logs" / "pwd-session.log";
    inline std::filesystem::path save = getAssetsBasePath() / "save" / "save.save";
}