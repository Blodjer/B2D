#pragma once

struct ApplicationConfig
{
    std::string name = "Game";
    uint32 windowWidth = 1280;
    uint32 windowHeight = 720;

    static void Dump(ApplicationConfig const& config)
    {
        B2D_CORE_INFO("-----------------------------------------------");
        B2D_CORE_INFO("name             {0}", config.name);
        B2D_CORE_INFO("windowWidth      {0}", config.windowWidth);
        B2D_CORE_INFO("windowHeight     {0}", config.windowHeight);
        B2D_CORE_INFO("-----------------------------------------------\n");
    }
};

namespace B2D
{
    void Config(ApplicationConfig& config);
}