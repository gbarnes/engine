#pragma once

#include "Core/GDI/GfxCmdList.h"
#include "Core/GDI/GfxVertexBuffer.h"

namespace Dawn
{
	class GfxCmdList;
	
	namespace RenderDebugInterface 
	{
		void Box();
		void Line();
		void Flush();
	}
}