#pragma once

#include "inc_common.h"
#include "inc_core.h"
#include "Core/GDI/inc_gfx_types.h"

namespace Dawn
{

	class DAWN_API Layer : public EObject
	{
	public:
		Layer();
		~Layer();

		virtual void Setup() = 0;
		virtual void Update() = 0;
		virtual void Render(ComPtr<ID3D12GraphicsCommandList2> InCmdList) = 0;
		virtual void Free() = 0;
	};
}

