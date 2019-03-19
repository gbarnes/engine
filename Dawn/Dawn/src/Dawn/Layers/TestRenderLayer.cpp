#include "TestRenderLayer.h"
#include "Core/GDI/GfxBackend.h"
#include "Core/GDI/GfxQueue.h"
#include "Core/GDI/GfxDevice.h"
#include "Core/GDI/inc_gfx_types.h"
#include <d3d12.h>
#include "Core/GDI/d3dx12.h"
#include "Application.h"
#include "inc_core.h"
#include "UI/UIEditorEvents.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader.h"

namespace Dawn
{
	struct VertexPosColor
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Color;
	};


	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, (size_t)(&((VertexPosColor*)0)->Position), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  (size_t)(&((VertexPosColor*)0)->Color), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> psBlob;


	void TestRenderLayer::OnFOVChanged(Event& InEvent)
	{
		UIFovChangedEvent& e = static_cast<UIFovChangedEvent&>(InEvent);
		this->FoV = e.GetFov();
	}

	void TestRenderLayer::OnCamPosChanged(Event& InEvent)
	{
		UICamPosChangedEvent& e = static_cast<UICamPosChangedEvent&>(InEvent);
		float* positionArray = e.GetPos();

		if (positionArray[0] == 0.0f && positionArray[1] == 0.0f && positionArray[2] == 0.0f)
			return;

		this->CamPosition[0] = positionArray[0];
		this->CamPosition[1] = positionArray[1];
		this->CamPosition[2] = positionArray[2];
	}

	u32 numOfIndices = 36;

	static WORD g_Indicis2[36] =
	{
		0, 1, 2, 0, 2, 3,
		4, 6, 5, 4, 7, 6,
		4, 5, 1, 4, 1, 0,
		3, 2, 6, 3, 6, 7,
		1, 5, 6, 1, 6, 2,
		4, 0, 3, 4, 3, 7
	};

	static VertexPosColor g_Vertices2[8] = {
		{ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0
		{ DirectX::XMFLOAT3(-1.0f,  1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 1
		{ DirectX::XMFLOAT3(1.0f,  1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 2
		{ DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) }, // 3
		{ DirectX::XMFLOAT3(-1.0f, -1.0f,  1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 4
		{ DirectX::XMFLOAT3(-1.0f,  1.0f,  1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 1.0f) }, // 5
		{ DirectX::XMFLOAT3(1.0f,  1.0f,  1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 6
		{ DirectX::XMFLOAT3(1.0f, -1.0f,  1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 1.0f) }  // 7
	};

	static VertexPosColor* g_Vertices = nullptr;
	static u16* g_Indices = nullptr;

	void TestRenderLayer::Setup()
	{
		CEventDispatcher::Subscribe(FOVChangedEvtKey, BIND_EVENT_MEMBER(TestRenderLayer::OnFOVChanged));
		CEventDispatcher::Subscribe(CamPosChangedEvtKey, BIND_EVENT_MEMBER(TestRenderLayer::OnCamPosChanged));

		
		u32 numOfVertices = 0;

		{
			tinyobj::attrib_t attrib;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;

			std::string warn;
			std::string err;
			bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "../Assets/Model/cube.obj", "../Assets/Model/", true );
			if (ret)
			{
				
				numOfVertices = attrib.vertices.size() / 3;
				g_Vertices = new VertexPosColor[numOfVertices];

				
				for (u32 i = 0; i < numOfVertices; ++i)
				{
					g_Vertices[i].Position = DirectX::XMFLOAT3(attrib.vertices[3 * i], attrib.vertices[3 * i + 1], attrib.vertices[3 * i + 2]);

					float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					g_Vertices[i].Color = DirectX::XMFLOAT3(r, r, r);
				}

				numOfIndices = 0;
				for (auto shape : shapes)
				{
					numOfIndices += shape.mesh.indices.size();
				}
				g_Indices = new u16[numOfIndices];

				
				u32 shapeCounter = 0;
				for (auto shape : shapes)
				{
					u8 count = 0;
					for (u32 i = 0; i < shape.mesh.indices.size(); ++i)
					{
						g_Indices[i] = shape.mesh.indices[i].vertex_index;
						i++;
						g_Indices[i] = shape.mesh.indices[i].vertex_index;
						i++;
						g_Indices[i] = shape.mesh.indices[i].vertex_index;
						i++;
						g_Indices[i] = shape.mesh.indices[i].vertex_index;
						i++;
						g_Indices[i] = shape.mesh.indices[i].vertex_index;
						i++;
						g_Indices[i] = shape.mesh.indices[i].vertex_index;
					}
				}

				DWN_INFO("Shape counter");
			}
			else
			{
				DWN_ERROR(err);
				return;
			}

		}

		ComPtr<ID3D12Device2> Device = GfxBackend::GetDevice()->GetD3D12Device();
		auto CommandQueue = GfxBackend::GetQueue(D3D12_COMMAND_LIST_TYPE_COPY);
		auto CommandList = CommandQueue->GetCommandList();
		CommandQueue->Flush();

		ComPtr<CGfxResource> intermediateVertexBuffer;
		GfxBackend::UpdateBufferResource
		(
			CommandList.Get(),
			&VertexBuffer,
			&intermediateVertexBuffer,
			numOfVertices,
			sizeof(VertexPosColor),
			g_Vertices,
			D3D12_RESOURCE_FLAG_NONE
		);

		VertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
		VertexBufferView.SizeInBytes = sizeof(VertexPosColor) * numOfVertices;
		VertexBufferView.StrideInBytes = sizeof(VertexPosColor);

		ComPtr<CGfxResource> intermediateIndexBuffer;
		GfxBackend::UpdateBufferResource
		(
			CommandList.Get(),
			&IndexBuffer,
			&intermediateIndexBuffer,
			numOfIndices,
			sizeof(u16),
			g_Indices,
			D3D12_RESOURCE_FLAG_NONE
		);

		IndexBufferView.BufferLocation = IndexBuffer->GetGPUVirtualAddress();
		IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
		IndexBufferView.SizeInBytes = sizeof(u16) * numOfIndices;

		ThrowIfFailed(GfxBackend::ReadShader(L"../bin/Debug-windows-x86_64/Sandbox/test_vs.cso", &vsBlob));
		ThrowIfFailed(GfxBackend::ReadShader(L"../bin/Debug-windows-x86_64/Sandbox/test_ps.cso", &psBlob));

		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		if (FAILED(Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		// Allow input layout and deny unnecessary access to certain pipeline stages.
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		// A single 32-bit constant root parameter that is used by the vertex shader.
		CD3DX12_ROOT_PARAMETER1 rootParameters[1];
		rootParameters[0].InitAsConstants(sizeof(DirectX::XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
		rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

		// Serialize the root signature.
		ComPtr<ID3DBlob> rootSignatureBlob;
		ComPtr<ID3DBlob> errorBlob;
		ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
			featureData.HighestVersion, &rootSignatureBlob, &errorBlob));

		// Create the root signature.
		ThrowIfFailed(Device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
			rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&RootSignature)));

		struct PipelineStateStream
		{
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		} pipelineStateStream;

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		pipelineStateStream = {};
		pipelineStateStream.pRootSignature = RootSignature.Get();
		pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
		pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
		pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		pipelineStateStream.RTVFormats = rtvFormats;
		

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
			sizeof(pipelineStateStream), &pipelineStateStream
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature = RootSignature.Get();
		desc.InputLayout = { inputLayout, _countof(inputLayout) };
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
		desc.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
		desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.NumRenderTargets = 1;
		desc.NodeMask = 0;
		desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		desc.RasterizerState.FillMode = D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID;
		desc.RasterizerState.CullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK;
		desc.RasterizerState.FrontCounterClockwise = false;
		desc.DepthStencilState.DepthEnable = false;
		desc.DepthStencilState.StencilEnable = false;
		desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		//desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		desc.SampleDesc.Count = 1;
		desc.SampleMask = UINT_MAX;

		//ThrowIfFailed(Device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&PipelineState)));

		ThrowIfFailed(Device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&PipelineState)));

		auto fenceValue = CommandQueue->ExecuteCommandList(CommandList);
		CommandQueue->WaitForFenceValue(fenceValue);
		CommandQueue->Flush();

		//delete g_Indices;
		//delete g_Vertices;
	}

	void TestRenderLayer::Update()
	{
		float angle = static_cast<float>(Timer::GetTime().GetTotalSeconds() * 90.0);
		const DirectX::XMVECTOR rotationAxis = DirectX::XMVectorSet(0, 1, 0, 0);
		ModelMatrix = DirectX::XMMatrixRotationAxis(rotationAxis, DirectX::XMConvertToRadians(angle));

		// Update the view matrix.
		const DirectX::XMVECTOR eyePosition = DirectX::XMVectorSet(CamPosition[0], CamPosition[1], CamPosition[2], 1);
		const DirectX::XMVECTOR focusPoint = DirectX::XMVectorSet(0, 0, 0, 1);
		const DirectX::XMVECTOR upDirection = DirectX::XMVectorSet(0, 1, 0, 0);
		ViewMatrix = DirectX::XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);

		SAppSettings* Settings = Application::GetSettings();

		// Update the projection matrix.
		float aspectRatio = Settings->WindowWidth / static_cast<float>(Settings->WindowHeight);
		ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(this->FoV), aspectRatio, 0.1f, 100.0f);
	}

	void TestRenderLayer::Render(ComPtr<CGfxCmdList> InCmdList)
	{
		CGfxState* state = PipelineState.Get();

		auto rtv = GfxBackend::GetCurrentBackbufferDescHandle();
		auto dsv = GfxBackend::GetDepthBufferDescHandle();
		InCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		InCmdList->IASetVertexBuffers(0, 1, &VertexBufferView);
		InCmdList->IASetIndexBuffer(&IndexBufferView);

		InCmdList->RSSetViewports(1, &GfxBackend::GetDevice()->GetViewport());
		InCmdList->RSSetScissorRects(1, &GfxBackend::GetDevice()->GetScissorRect());

		InCmdList->SetPipelineState(state);
		InCmdList->SetGraphicsRootSignature(RootSignature.Get());

		DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(ModelMatrix, ViewMatrix);
		mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, ProjectionMatrix);
		InCmdList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &mvpMatrix, 0);
		InCmdList->DrawIndexedInstanced(numOfIndices, 1, 0, 0, 0);
	}

	void TestRenderLayer::Free()
	{
		delete g_Indices;
		delete g_Vertices;
		this->PipelineState.Reset();
		this->IndexBuffer.Reset();
		this->VertexBuffer.Reset();
		this->RootSignature.Reset();
	}
}