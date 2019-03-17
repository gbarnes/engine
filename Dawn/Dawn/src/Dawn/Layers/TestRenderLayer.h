#pragma once

#include "Layer.h"
#include "Core/GDI/inc_gfx_types.h"

namespace Dawn
{

	class CTestRenderLayer : public CLayer
	{
	public:
		void Setup();
		void Update();
		void Render(ComPtr<CGfxCmdList> InCmdList);
		void Free();
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
	};

}