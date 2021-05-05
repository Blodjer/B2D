-- This file has to be included in the actual project.
-- A premake file for your project should start like this:
--[[
workspace "YourWorkspaceName"
	startproject "YourProjectName"
	include "B2D"

	project "YourProjectName"
		...
]]--

if premake.api.scope.workspace == nil then
workspace "B2D"
end
	cppdialect "C++17"
	systemversion "latest"

	targetdir "%{wks.location}/Bin/%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}"
	objdir "%{wks.location}/Tmp/%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}/%{prj.name}"

	flags { "MultiProcessorCompile" }

	configurations
	{
		"Debug",
		"Profile",
		"Release"
	}

	platforms
	{
		"Win32",
		"Win64"
	}

	defines
	{
		"_ITERATOR_DEBUG_LEVEL=0"
	}
	
	filter "configurations:Debug"
		symbols "On"
		optimize "Off"
		defines "B2D_BUILD_DEBUG"

	filter "configurations:Profile"
		symbols "Off"
		optimize "On"
		defines "B2D_BUILD_PROFILE"

	filter "configurations:Release"
		symbols "Off"
		optimize "On"
		defines "B2D_BUILD_RELEASE"

	filter "platforms:Win32"
		system "Windows"
		architecture "x86"
		defines "B2D_PLATFORM_WINDOWS_X86"

	filter "platforms:Win64"
		system "Windows"
		architecture "x64"
		defines "B2D_PLATFORM_WINDOWS_X64"

	filter "system:Windows"
		defines "B2D_PLATFORM_DESKTOP"
		defines "B2D_PLATFORM_WINDOWS"
	

group "Libraries"
	include "Libraries/glew"
	include "Libraries/GLFW"
	include "Libraries/imgui"
group ""


project "B2D"
	language "C++"
	kind "StaticLib"
	staticruntime "On"

	pchheader "B2D_pch.h"
	pchsource "%{prj.location}/Source/B2D_pch.cpp"

	files
	{
		"%{prj.location}/Source/**"
	}

	includedirs
	{
		"%{prj.location}/Source/",
		"%{prj.location}/Libraries/",
		"%{prj.location}/Libraries/glew/include/",
		"%{prj.location}/Libraries/GLFW/include/",
		"%{prj.location}/Libraries/glm/",
		"%{prj.location}/Libraries/spdlog/include/",
		"$(VULKAN_SDK)/Include/"
	}

	defines
	{
		"GLEW_STATIC"
	}

	links
	{
		"glew",
		"GLFW",
		"imgui",
		"opengl32.lib",
		"dbghelp.lib"
	}

	filter "platforms:Win64"
		links "$(VULKAN_SDK)/Lib/vulkan-1.lib"

	filter "platforms:Win32"
		links "$(VULKAN_SDK)/Lib32/vulkan-1.lib"

	filter "system:windows"
		defines "WIN32_LEAN_AND_MEAN"
		defines "VK_USE_PLATFORM_WIN32_KHR"
		buildoptions ("/sdl")