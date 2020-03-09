workspace "Dawn"
	architecture "x64"

	configurations 
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
physicsdir = "Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/"

include "Dawn/vendor/imgui"
include "Dawn/vendor/DirectXTex"
include "Dawn/vendor/brofiler"
include "Dawn/vendor/glad"
include "Dawn/vendor/pugixml"

project "Dawn"
	location "Dawn"
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	pchheader "stdafx.h"
	pchsource "Dawn/src/Dawn/stdafx.cpp"
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",--,
		"Dawn/src/Dawn/stdafx.h",
		"Dawn/src/Dawn/stdafx.cpp"
		--"%{prj.name}/shader/**.hlsl"
	}

	
	includedirs
	{
		"Dawn/vendor/spdlog/include/",
		"Dawn/vendor/stb/include/",
		"Dawn/vendor/imgui/",
		"Dawn/vendor/glm/",
		"Dawn/vendor/DirectXTex/",
		"Dawn/vendor/brofiler/src/",
		"Dawn/vendor/TaskScheduler/Scheduler/Include/",
		"Dawn/vendor/assimp/include/",
		"Dawn/vendor/glad/glad/",
		"Dawn/vendor/pugixml/src/",
		"Dawn/src/Dawn/",
		"Dawn/vendor/physx/physx/include/",
		"Dawn/vendor/physx/physx/include/**",
		"Dawn/vendor/physx/pxshared/include/",
		"Dawn/vendor/DirectX/Include/"
	}
	
	links 
	{
		
	}
	
	libdirs { "Dawn/vendor/assimp/lib/x64/*", "Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/*", "Dawn/vendor/DirectX/Lib/*" }

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines 
		{
			"DWN_PLATFORM_WINDOWS",
			"DAWN_BUILD_DLL"
		}
		

		postbuildcommands 
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox"),
			("{COPY} vendor/assimp/bin/x64 ../bin/" .. outputdir .. "/Sandbox"),
			("{COPY} vendor/Physx/physx/bin/win.x86_64.vc141.mt/debug/PhysX_64.dll ../bin/" .. outputdir .. "/Sandbox"),
			("{COPY} vendor/Physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXCommon_64.dll ../bin/" .. outputdir .. "/Sandbox"),
			("{COPY} vendor/Physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXFoundation_64.dll ../bin/" .. outputdir .. "/Sandbox"),
			("{COPY} vendor/Physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXCooking_64.dll ../bin/" .. outputdir .. "/Sandbox"),
			("{COPY} vendor/Physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXGPU_64.dll ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug" 
		cppdialect "C++17"
		defines "DAWN_DEBUG"
		symbols "On"
		optimize "Off"

	filter "configurations:Release" 
		flags {LinkTimeOptimization}
		cppdialect "C++17"
		defines "DAWN_RELEASE;NDEBUG"
		symbols "Off"
		optimize "Full"

	filter "configurations:Dist" 
		flags {LinkTimeOptimization}
		cppdialect "C++17"
		defines "DAWN_DIST;NDEBUG"
		symbols "Off"
		optimize "Speed"

    -- shadermodel("5.0")

    -- shaderassembler("AssemblyCode")

    -- local shader_dir = "../Assets/Shader/"

   -- HLSL files that don't end with 'Extensions' will be ignored as they will be
   -- used as includes
   -- filter("files:**.hlsl")
     --  flags("ExcludeFromBuild")
   --    shaderobjectfileoutput(shader_dir.."%{file.basename}"..".cso")
    --   shaderassembleroutput(shader_dir.."%{file.basename}"..".asm")

   -- filter("files:**_ps.hlsl")
   --    removeflags("ExcludeFromBuild")
   --    shadertype("Pixel")

   -- filter("files:**_vs.hlsl")
   --    removeflags("ExcludeFromBuild")
   --    shadertype("Vertex")


   -- Warnings as errors
   -- shaderoptions({"/WX"})

project "Sandbox"
	location "Sandbox"
	kind "WindowedApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	pchheader "pch.h"
	pchsource "Sandbox/src/pch.cpp"
	linkoptions { '/NODEFAULTLIB:"libcmt.lib"' }
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"Sandbox/src/pch.h",
		"Sandbox/src/pch.cpp"
	}

	includedirs
	{
		"Dawn/vendor/spdlog/include/",
		"Dawn/vendor/stb/include/",
		"Dawn/src/Dawn/",
		"Dawn/vendor/DirectXTex/",
		"Dawn/vendor/glad/glad/",
		"Dawn/vendor/brofiler/src/",
		"Dawn/vendor/TaskScheduler/Scheduler/Include/",
		"Dawn/vendor/assimp/include/",
		"Dawn/vendor/glm/",
		"Dawn/vendor/pugixml/src/",
		"Dawn/vendor/Physx/physx/include/",
		"Dawn/vendor/Physx/physx/include/**",
		"Dawn/vendor/Physx/pxshared/include/",
		"Dawn/vendor/DirectX/Include/",
		"Sandbox/src/"
	}
	
	links 
	{
		"ImGui",
		"DirectXTex",
		"Brofiler",
		"glad",
		"pugixml",
		"Dawn/vendor/Physx/physx/bin/win.x86_64.vc141.mt/debug/PhysX_64.lib",
		"Dawn/vendor/Physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXCommon_64.lib",
		"Dawn/vendor/Physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXFoundation_64.lib",
		"Dawn/vendor/Physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXCooking_64.lib",
		"Dawn/vendor/Physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXExtensions_static_64.lib",
		"Dawn/vendor/Physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXPvdSDK_static_64.lib",
		"Dawn/vendor/Physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXTask_static_64.lib",
		"Dawn/vendor/Physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXVehicle_static_64.lib",
		"Dawn/vendor/Physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXCharacterKinematic_static_64.lib",
		"Dawn/vendor/Physx/physx/bin/win.x86_64.vc141.mt/debug/LowLevelDynamics_static_64.lib",
		"Dawn/vendor/Physx/physx/bin/win.x86_64.vc141.mt/debug/LowLevelAABB_static_64.lib",
		"Dawn/vendor/Physx/physx/bin/win.x86_64.vc141.mt/debug/FastXml_static_64.lib",
		"Dawn/vendor/Physx/physx/bin/win.x86_64.vc141.mt/debug/LowLevel_static_64.lib",
		"Dawn/vendor/assimp/lib/x64/assimp-vc140-mt.lib",
		"Dawn/vendor/DirectX/Lib/*",
		"Dawn"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines 
		{
			"DWN_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug" 
		defines "DAWN_DEBUG"
		symbols "On"
		optimize "Off"

	filter "configurations:Release" 
		flags {LinkTimeOptimization}
		defines "DAWN_RELEASE;NDEBUG"
		symbols "On"
		optimize "Full"

	filter "configurations:Dist" 
		flags {LinkTimeOptimization}
		defines "DAWN_DIST;NDEBUG"
		symbols "On"
		optimize "Speed"