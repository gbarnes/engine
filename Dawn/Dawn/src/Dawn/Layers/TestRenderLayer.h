#pragma once

#include "Layer.h"
#include "Core/GDI/inc_gfx_types.h"

namespace Dawn
{
	class GfxCmdList;
	class Application;

	class TestRenderLayer : public Layer
	{
	public:
		TestRenderLayer(Shared<Dawn::Application> InApplication);

		void Setup();
		void Update(float InDeltaTime);
		void Render();
		void Free();

	private:
		mat4 View;
		mat4 Model;
		mat4 Perspective;
		float FoV = 45.0f;
		float CamPosition[3] = { 0, 3, 10 };
	};

}