#pragma once

#include "Core/Core.h"

class World;

struct ApplicationConfig
{
    std::string name = "Game";
    uint32 windowWidth = 1600;
    uint32 windowHeight = 900;
    bool multithread = true;

    static void Dump(ApplicationConfig const& config)
    {
        B2D_LOG_INFO("--- ApplicationConfig -------------------");
        B2D_LOG_INFO("name            {}", config.name);
        B2D_LOG_INFO("windowWidth     {}", config.windowWidth);
        B2D_LOG_INFO("windowHeight    {}", config.windowHeight);
        B2D_LOG_INFO("multithread     {}", config.multithread);
        B2D_LOG_INFO("-----------------------------------------\n");
    }
};

namespace B2D
{
    void Config(ApplicationConfig& config);
    void PopulateWorld(World* const world);
}