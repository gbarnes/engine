#pragma once

namespace Dawn
{
	namespace Paths
	{
		//
		// Returns the engines root dir
		//
		FORCEINLINE Path EngineDir()
		{
			return std::filesystem::current_path();
		}

		//
		// Returns the project root dir
		//
		FORCEINLINE Path ProjectDir()
		{
			Path WorkingPath = std::filesystem::current_path();
			return WorkingPath.parent_path();
		}

		//
		// Returns the project content dir
		//
		FORCEINLINE Path ProjectContentDir()
		{
			return ProjectDir().append("Assets");
		}

		//
		// Returns the project content dir
		//
		FORCEINLINE Path ProjectLogDir()
		{
			return ProjectDir().append("Logs");
		}

		//
		// Returns the project config dir
		//
		FORCEINLINE Path ProjectConfigDir()
		{
			return ProjectDir().append("Config");
		}

		//
		// Returns the project config dir
		//
		FORCEINLINE Path ProjectShaderDir()
		{
			return ProjectContentDir().append("Shader");
		}
	}
}