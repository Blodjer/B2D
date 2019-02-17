mLibdir = "%{prj.location}/Lib/%{cfg.architecture}/"

group "Libraries"
	include "Libraries/glew"
	include "Libraries/GLFW"
group ""

project "B2D"
	language "C++"
	kind "StaticLib"

	libdirs (mLibdir)

	files
	{
		"%{prj.location}/Source/**"
	}

	includedirs
	{
		"%{prj.location}/Source/",
		"%{prj.location}/Include/",
		"%{prj.location}/Libraries/",
		"%{prj.location}/Libraries/glew/include/",
		"%{prj.location}/Libraries/GLFW/include/",
		"%{prj.location}/Libraries/glm/"
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