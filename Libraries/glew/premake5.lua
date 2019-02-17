project "glew"
    kind "StaticLib"
    language "C"

	files
	{
        "src/*.c",
    }

    includedirs
	{
		"include/"
    }
    
    defines
    {
        "GLEW_STATIC",
        "WIN32",
        "_LIB",
        "WIN32_LEAN_AND_MEAN"
    }

	filter "system:windows"
        systemversion "latest"
        staticruntime "On"

        filter "configurations:Debug"
            optimize "Off"

        filter "configurations:Release"
            optimize "Speed"
            inlining "Explicit"
            buildoptions { "/GF" }