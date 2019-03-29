#pragma once

#include "Core/Core.h"

struct ApplicationConfig
{
    std::string name = "Game";
    uint32 windowWidth = 1280;
    uint32 windowHeight = 720;

    static void Dump(ApplicationConfig const& config)
    {
        B2D_CORE_INFO("--- ApplicationConfig -------------------");
        B2D_CORE_INFO("name            {}", config.name);
        B2D_CORE_INFO("windowWidth     {}", config.windowWidth);
        B2D_CORE_INFO("windowHeight    {}", config.windowHeight);
        B2D_CORE_INFO("-----------------------------------------\n");
    }
};

namespace B2D
{
    void Config(ApplicationConfig& config);
    void PopulateWorld(World* const world);
}