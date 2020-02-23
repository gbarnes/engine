#pragma once
#include "Core/GDI/Base/inc_gfx_types.h"
#include "Core/Container/Map.h"

namespace Dawn
{
	class Application;

	class PSOCache
	{
	public:
		static void CreateDefaults(Application* InApplication);
		static void Cache(const std::string& InName, Dawn::GfxResId InId);
		static Dawn::GfxResId GetCached(const std::string& InName);
	private:
		static Map<std::string, Dawn::GfxResId> CachedObjects;
	};

}