#pragma once
#include "inc_common.h"
#include "inc_core.h"
#include <filesystem>

namespace Dawn
{
	namespace Config
	{
		//
		// Takes care of loading the configs with the passed names
		// Names are to be passed without extension (Example: Engine.ini becomes Engine).
		//
		void Load(std::initializer_list<std::string> InConfigNames);

		//
		// Takes care of saving the configs with the passed names
		// Names are to be passed without extension (Example: Engine.ini becomes Engine).
		//
		void Save(std::initializer_list<std::string> InConfigNames);

		bool GetInt(const std::string& InConfigName, const std::string& InKey, int* OutValue);
		bool GetBool(const std::string& InConfigName, const std::string& InKey, bool* OutValue);
		bool GetFloat(const std::string& InConfigName, const std::string& InKey, float* OutValue);
		bool GetString(const std::string& InConfigName, const std::string& InKey, std::string* OutValue);

		void SetInt(const std::string& InConfigName, const std::string& InKey, int InValue);
		void SetFloat(const std::string& InConfigName, const std::string& InKey, float InValue);
		void SetString(const std::string& InConfigName, const std::string& InKey, const std::string& InValue);
		void SetBool(const std::string& InConfigName, const std::string& InKey, bool InValue);
	}
}
