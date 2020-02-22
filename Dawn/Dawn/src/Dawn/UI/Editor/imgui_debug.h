#pragma once

class GfxRenderBuffer;

namespace Dawn
{
	void ShowGBuffer(GfxRenderBuffer* InBuffer, GfxRenderBuffer* InShadowBuffer, GfxRenderBuffer* InSSAOBuffer);
	void ShowFpsCounter();
	void OutputDebugText(const std::string& InText);
	void ShowDebugText();
	void ShowFramegraph();
}