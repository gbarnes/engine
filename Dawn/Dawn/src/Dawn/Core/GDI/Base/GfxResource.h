#pragma once
#include "inc_gfx_types.h"
#include "GfxGDI.h"

namespace Dawn
{
	class GfxResource
	{
	public:
		GfxResource(const GfxResId& InId, GfxGDIPtr InGDI)
			: Id(InId), GDI(InGDI)
		{
		}

		virtual ~GfxResource()
		{
			GDI = nullptr;
		}

		inline GfxResId GetId() const
		{
			return Id;
		}

		virtual inline void SetName(const std::string& InName)
		{
			Name = InName;
		}

		inline std::string GetName() const
		{
			return Name;
		}


	protected:
		std::string Name;
		GfxResId Id;
		GfxGDIPtr GDI;
	};
}