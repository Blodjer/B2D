group "Libraries"
	include "Libraries/glew"
	include "Libraries/GLFW"
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
		"%{prj.location}/Libraries/spdlog/include/"
	}

	defines
	{
		"GLEW_STATIC"
	}

	links
	{
		"GLFW",
		"glew",
		"opengl32.lib"
	}

	filter "system:windows"
		buildoptions ("/sdl")