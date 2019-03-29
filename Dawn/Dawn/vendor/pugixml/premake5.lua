project "pugixml"
    kind "StaticLib"
    language "C++"
    
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
        "src/pugixml.hpp",
		"src/pugiconfig.hpp",
		"src/pugixml.cpp"
    }

	filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "On"
     
	filter { "system:windows", "configurations:Debug" }
        buildoptions "/MTd" 

    filter { "system:windows", "configurations:Release" }
        buildoptions "/MT"
