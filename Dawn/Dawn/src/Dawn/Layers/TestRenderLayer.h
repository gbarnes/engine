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
		void Render(ComPtr<ID3D12GraphicsCommandList2> InCmdList);
		void Free();

		void OnFOVChanged(Event& InEvent);
		void OnCamPosChanged(Event& InEvent);

	private:
		// Vertex buffer for the cube.
		ComPtr<ID3D12Resource> VertexBuffer;
		CGfxVertexView VertexBufferView;

		// Index buffer for the cube.
		ComPtr<ID3D12Resource> GfxIndexBuffer;
		CGfxIndexView IndexBufferView;

		ComPtr<ID3D12RootSignature> RootSignature;
		ComPtr<ID3D12PipelineState> PipelineState;

		DirectX::XMMATRIX ModelMatrix;
		DirectX::XMMATRIX ViewMatrix;
		DirectX::XMMATRIX ProjectionMatrix;

		float FoV = 45.0f;
		float CamPosition[3] = { 0, 3, -10 };
	};

}