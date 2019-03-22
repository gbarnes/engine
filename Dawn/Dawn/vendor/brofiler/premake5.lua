IncludeDir = {}
IncludeDir["Scheduler"] = "../TaskScheduler/Scheduler"

include "../TaskScheduler/Scheduler"

project "Brofiler"
    kind "StaticLib"
    language "C++"
    
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
        "src/**.h",
		"src/**.cpp",
    }

	includedirs
	{
		"%{IncludeDir.Scheduler}/Include/",
		"src/"
	}

	links 
	{
		"Scheduler"
	}
    
	filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "On"
        
    filter "configurations:Debug" 
		cppdialect "C++17"
		symbols "On"

	filter "configurations:Release" 
		cppdialect "C++17"
		symbols "On"

	filter "configurations:Dist" 
		cppdialect "C++17"
		symbols "On"
