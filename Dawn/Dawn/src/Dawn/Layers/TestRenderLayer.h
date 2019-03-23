#pragma once

#include "Layer.h"
#include "Core/GDI/inc_gfx_types.h"
#include "Core/GDI/GfxRootSignature.h"

namespace Dawn
{
	class GfxCmdList;

	class TestRenderLayer : public Layer
	{
	public:
		void Setup();
		void Update();
		void Render(GfxCmdList* InCmdList);
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
		GfxRootSignature RootSignature;

		//ComPtr<ID3D12RootSignature> RootSignature;
		ComPtr<ID3D12PipelineState> PipelineState;

		DirectX::XMMATRIX ModelMatrix;
		DirectX::XMMATRIX ViewMatrix;
		DirectX::XMMATRIX ProjectionMatrix;

		float FoV = 45.0f;
		float CamPosition[3] = { 0, 3, -10 };
	};

}