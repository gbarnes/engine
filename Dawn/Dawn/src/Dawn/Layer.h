#pragma once

#include "inc_common.h"
#include "inc_core.h"
#include "Core/GDI/inc_gfx_types.h"

namespace Dawn
{

	class DAWN_API CLayer : public CEObject
	{
	public:
		CLayer();
		~CLayer();

		virtual void Setup() = 0;
		virtual void Update() = 0;
		virtual void Render(ComPtr<CGfxCmdList> InCmdList) = 0;
		virtual void Free() = 0;
	};
}

