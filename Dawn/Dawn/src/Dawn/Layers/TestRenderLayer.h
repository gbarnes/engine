#pragma once

#include "Layer.h"
#include "Core/GDI/inc_gfx_types.h"

namespace Dawn
{

	class TestRenderLayer : public Layer
	{
	public:
		void Setup();
		void Update();
		void Render(ComPtr<CGfxCmdList> InCmdList);
		void Free();

		void OnFOVChanged(Event& InEvent);
		void OnCamPosChanged(Event& InEvent);

	private:
		// Vertex buffer for the cube.
		ComPtr<CGfxResource> VertexBuffer;
		CGfxVertexView VertexBufferView;

		// Index buffer for the cube.
		ComPtr<CGfxResource> IndexBuffer;
		CGfxIndexView IndexBufferView;

		ComPtr<ID3D12RootSignature> RootSignature;
		ComPtr<CGfxState> PipelineState;

		DirectX::XMMATRIX ModelMatrix;
		DirectX::XMMATRIX ViewMatrix;
		DirectX::XMMATRIX ProjectionMatrix;

		float FoV = 45.0f;
		float CamPosition[3] = { 0, 3, -10 };
	};

}