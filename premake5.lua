mLibdir = "%{prj.location}/Lib/%{cfg.architecture}/"

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
		"%{prj.location}/Include/",
		"%{prj.location}/Source/"
	}

	defines
	{
		"GLEW_STATIC"
	}

	links
	{
		"glfw3.lib",
		"glew32s.lib",
		"opengl32.lib",
		"libpng16.lib"
	}

	postbuildcommands
	{
		("{COPY} \"" .. mLibdir .. "*.dll\" \"%{cfg.targetdir}/\"")
	}

	filter "system:windows"
		buildoptions ("/sdl")