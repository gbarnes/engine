#include "Config.h"

namespace Dawn
{
	struct ConfigFile
	{
		std::string Name;
		std::map<std::string, std::string> Values;
	};
	std::map<std::string, ConfigFile> g_ConfigFiles;

	std::string* FindValue(const std::string& InConfigName, const std::string& InKey)
	{
		const auto Iterator = g_ConfigFiles.find(InConfigName);
		if (Iterator == g_ConfigFiles.end())
		{
			return nullptr;
		}

		auto& Config = Iterator->second;
		auto& KeyPair = Config.Values.find(InKey);
		if (KeyPair == Config.Values.end())
		{
			return nullptr;
		}

		return &KeyPair->second;
	}

	void CreateValue(const std::string& InConfigName, const std::string& InKey, const std::string& InValue)
	{
		const auto Iterator = g_ConfigFiles.find(InConfigName);
		if (Iterator == g_ConfigFiles.end())
		{
			return;
		}

		auto& Config = Iterator->second;
		Config.Values.emplace(std::make_pair(InKey, InValue));
	}

	void Config::Load(std::initializer_list<std::string> InFiles)
	{
		const auto Path = Paths::ProjectConfigDir();

		// note - can we optimize parsing the ini files 
		//		  by somehow make looking for section 
		//		  in the file more efficient? 
		//		  For now this is a pretty straight forward solution.
		//		  On the other hand do we need optimization at all? 
		//		  I don't think that we will push a lot of config values anyways.
		//		  (gb, 10/11/19)

		for (const std::string& File : InFiles)
		{
			auto FilePath = Path;
			FilePath.append((File + ".ini").c_str());

			FILE* IniFile;
			i32 errorno = fopen_s(&IniFile, FilePath.string().c_str(), "r");
			if (IniFile)
			{
				ConfigFile Config = {};
				Config.Name = File;

				u64 Size = -1;
				bool bIsParsingSection = false;
				std::string CurrentData;
				std::string LastSection;
				std::string LastEntry;
				char Buffer[1];
				char CurrentChar;

				while (Size != 0)
				{
					Size = fread_s(&Buffer, sizeof(Buffer), sizeof(char), 1, IniFile);

					if (Size > 0)
					{
						CurrentChar = Buffer[0];

						if (CurrentChar == '[')
						{
							LastSection = "";
							bIsParsingSection = true;
						}
						else if (CurrentChar == ']')
						{
							LastSection = CurrentData;
							CurrentData = "";
						}
						else if (CurrentChar == '=')
						{
							std::string configKey = LastSection + "/" + CurrentData;
							auto Section = Config.Values.find(configKey);
							if (Section == Config.Values.end())
							{
								Config.Values.insert(std::make_pair(configKey, ""));
								LastEntry = configKey;
							}
							else
							{
								LastEntry = "";
							}

							CurrentData = "";
						}
						else if (CurrentChar == '\n')
						{
							if (!bIsParsingSection && !LastEntry.empty())
							{
								auto Section = Config.Values.find(LastEntry);
								if (Section != Config.Values.end())
								{
									Section->second = CurrentData;
								}
							}

							LastEntry = "";
							CurrentData = "";
							bIsParsingSection = false;
						}
						else
						{
							CurrentData += CurrentChar;
						}
					}
				}

				if (!bIsParsingSection && !LastEntry.empty())
				{
					auto Section = Config.Values.find(LastEntry);
					if (Section != Config.Values.end())
					{
						Section->second = CurrentData;
					}
				}

				g_ConfigFiles.insert(std::make_pair(Config.Name, Config));
				fclose(IniFile);
			}
		}
	}

	void Config::Save(std::initializer_list<std::string> InConfigNames)
	{
		for (auto ConfigName : InConfigNames)
		{
			auto It = g_ConfigFiles.find(ConfigName);
			if (It == g_ConfigFiles.end())
				continue;

			FILE* File;
			Path FullPath = Paths::ProjectConfigDir().append(ConfigName + ".ini");
			size_t errorno = fopen_s(&File, FullPath.string().c_str(), "r+");
			if (errorno > 0)
				continue;

			std::string FileContent = "";
			std::string CurrentSection;
			bool bIsFirstLine = true;
			auto& Config = It->second;
			for (auto ValueIt = Config.Values.begin(); ValueIt != Config.Values.end(); ++ValueIt)
			{
				std::string SectionAndKey = ValueIt->first;
				std::string Value = ValueIt->second;

				// Sections and Keys are stored like this SectionName/KeyName
				// Split these into separate parts.
				u64 Offset = SectionAndKey.find('/');
				std::string SectionPart = SectionAndKey.substr(0, Offset);
				std::string Key = SectionAndKey.substr(Offset+1, SectionAndKey.size());

				if (CurrentSection.empty() || CurrentSection != SectionPart) 
				{
					std::string NewLines = (bIsFirstLine) ? "" : "\n";
					fputs(std::string(NewLines + "[" + SectionPart + "]\n").c_str(), File);
					CurrentSection = SectionPart;
				}
				
				fputs(std::string(Key + "=" + Value + "\n").c_str(), File);

				bIsFirstLine = false;
			}

			fclose(File);
		}
	}

	bool Config::GetInt(const std::string& InConfigName, const std::string& InKey, int* OutValue)
	{
		std::string* Value = FindValue(InConfigName, InKey);
		if (Value == nullptr)
			return nullptr;

		*OutValue = std::stoi(*Value);
		return true;
	}

	bool Config::GetFloat(const std::string& InConfigName, const std::string& InKey, float* OutValue)
	{
		std::string* Value = FindValue(InConfigName, InKey);
		if (Value == nullptr)
			return nullptr;

		*OutValue = std::stof(*Value);
		return true;
	}

	bool Config::GetString(const std::string& InConfigName, const std::string& InKey, std::string* OutValue)
	{
		std::string* Value = FindValue(InConfigName, InKey);
		if (Value == nullptr)
			return nullptr;

		(*OutValue) = *Value;
		return true;
	}

	bool Config::GetBool(const std::string& InConfigName, const std::string& InKey, bool* OutValue)
	{
		std::string* Value = FindValue(InConfigName, InKey);
		if (Value == nullptr)
			return nullptr;

		(*OutValue) = std::stoi(*Value);
		return true;
	}

	void Config::SetInt(const std::string& InConfigName, const std::string& InKey, int InValue)
	{
		std::string* Value = FindValue(InConfigName, InKey);
		if (Value == nullptr)
		{
			CreateValue(InConfigName, InKey, std::to_string(InValue));
			return;
		}

		(*Value) = std::to_string(InValue);
	}

	void Config::SetFloat(const std::string& InConfigName, const std::string& InKey, float InValue)
	{
		std::string* Value = FindValue(InConfigName, InKey);
		if (Value == nullptr)
		{
			CreateValue(InConfigName, InKey, std::to_string(InValue));
			return;
		}

		(*Value) = std::to_string(InValue);
	}

	void Config::SetString(const std::string& InConfigName, const std::string& InKey, const std::string& InValue)
	{
		std::string* Value = FindValue(InConfigName, InKey);
		if (Value == nullptr) 
		{
			CreateValue(InConfigName, InKey, InValue);
			return;
		}

		(*Value) = InValue;
	}

	void Config::SetBool(const std::string& InConfigName, const std::string& InKey, bool InValue)
	{
		std::string* Value = FindValue(InConfigName, InKey);
		if (Value == nullptr)
		{
			CreateValue(InConfigName, InKey, std::to_string(InValue));
			return;
		}

		(*Value) = std::to_string(InValue);
	}
}