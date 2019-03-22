workspace "Dawn"
	architecture "x64"

	configurations 
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["ImGui"] = "Dawn/vendor/imgui"
IncludeDir["DirectXTex"] = "Dawn/vendor/DirectXTex"

include "Dawn/vendor/imgui"
include "Dawn/vendor/DirectXTex"

project "Dawn"
	location "Dawn"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/shader/**.hlsl"
	}

	includedirs
	{
		"Dawn/vendor/spdlog/include/",
		"Dawn/vendor/tinyobjloader/include/",
		"Dawn/vendor/stb/include/",
		"%{prj.name}/src/Dawn/",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.DirectXTex}"
	}

	links 
	{
		"ImGui",
		"DirectXTex"
	}

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
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
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


    shadermodel("5.0")

    shaderassembler("AssemblyCode")

    local shader_dir = "../Assets/Shader/"

   -- HLSL files that don't end with 'Extensions' will be ignored as they will be
   -- used as includes
   filter("files:**.hlsl")
      flags("ExcludeFromBuild")
      shaderobjectfileoutput(shader_dir.."%{file.basename}"..".cso")
      shaderassembleroutput(shader_dir.."%{file.basename}"..".asm")

   filter("files:**_ps.hlsl")
      removeflags("ExcludeFromBuild")
      shadertype("Pixel")

   filter("files:**_vs.hlsl")
      removeflags("ExcludeFromBuild")
      shadertype("Vertex")


   -- Warnings as errors
   shaderoptions({"/WX"})

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
		"%{IncludeDir.DirectXTex}",
	}

	links 
	{
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