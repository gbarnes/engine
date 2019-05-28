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
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"--,
		--"%{prj.name}/shader/**.hlsl"
	}

	includedirs
	{
		"Dawn/vendor/spdlog/include/",
		"Dawn/vendor/tinyobjloader/include/",
		"Dawn/vendor/stb/include/",
		"Dawn/vendor/imgui/",
		"Dawn/vendor/glm/",
		"Dawn/vendor/DirectXTex/",
		"Dawn/vendor/brofiler/src/",
		"Dawn/vendor/assimp/include/",
		"Dawn/vendor/glad/glad/",
		"Dawn/vendor/pugixml/src/",
		"%{prj.name}/src/Dawn/",
		"Dawn/vendor/physx/physx/include/",
		"Dawn/vendor/physx/physx/include/**",
		"Dawn/vendor/physx/pxshared/include/"
	}
	
	links 
	{
		"ImGui",
		"DirectXTex",
		"Brofiler",
		"glad",
		"pugixml",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/PhysX_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXCommon_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXFoundation_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXCooking_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXExtensions_static_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXPvdSDK_static_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXTask_static_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXVehicle_static_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXCharacterKinematic_static_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/LowLevelDynamics_static_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/LowLevelAABB_static_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/FastXml_static_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/LowLevel_static_64.lib",
		"Dawn/vendor/assimp/lib/x64/assimp-vc140-mt.lib"
	}
	
	libdirs { "Dawn/vendor/assimp/lib/x64/*", "Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/*"  }

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
			("{COPY} vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/PhysX_64.dll ../bin/" .. outputdir .. "/Sandbox"),
			("{COPY} vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXCommon_64.dll ../bin/" .. outputdir .. "/Sandbox"),
			("{COPY} vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXFoundation_64.dll ../bin/" .. outputdir .. "/Sandbox"),
			("{COPY} vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXCooking_64.dll ../bin/" .. outputdir .. "/Sandbox"),
			("{COPY} vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXGPU_64.dll ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug" 
		cppdialect "C++17"
		defines "DAWN_DEBUG"
		symbols "On"

	filter "configurations:Release" 
		cppdialect "C++17"
		defines "DAWN_RELEASE"
		symbols "On"

	filter "configurations:Dist" 
		cppdialect "C++17"
		defines "DAWN_DIST"
		symbols "On"


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
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Dawn/vendor/spdlog/include/",
		"Dawn/vendor/tinyobjloader/include/",
		"Dawn/vendor/stb/include/",
		"Dawn/src",
		"Dawn/vendor/DirectXTex/",
		"Dawn/vendor/glad/glad/",
		"Dawn/vendor/brofiler/src/",
		"Dawn/vendor/assimp/include/",
		"Dawn/vendor/glm/",
		"Dawn/vendor/pugixml/src/",
		"Dawn/vendor/physx/physx/include/",
		"Dawn/vendor/physx/physx/include/**",
		"Dawn/vendor/physx/pxshared/include/"
	}
	
	links 
	{
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/PhysX_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXCommon_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXFoundation_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXCooking_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXExtensions_static_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXPvdSDK_static_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXTask_static_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXVehicle_static_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/PhysXCharacterKinematic_static_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/LowLevelDynamics_static_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/LowLevelAABB_static_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/FastXml_static_64.lib",
		"Dawn/vendor/physx/physx/bin/win.x86_64.vc141.mt/debug/LowLevel_static_64.lib",
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

	filter "configurations:Release" 
		defines "DAWN_RELEASE"
		symbols "On"

	filter "configurations:Dist" 
		defines "DAWN_DIST"
		symbols "On"