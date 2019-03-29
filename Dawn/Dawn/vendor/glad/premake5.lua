project "glad"
    kind "StaticLib"
    language "C++"
    
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
        "glad/**.h",
		"glad/**.c",
    }

	includedirs
	{
		"glad/",
    }
    
	filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "On"
     
	filter { "system:windows", "configurations:Debug" }
        buildoptions "/MTd" 

    filter { "system:windows", "configurations:Release" }
        buildoptions "/MT"
