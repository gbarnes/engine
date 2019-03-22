#include "TestRenderLayer.h"
#include "Core/GDI/GfxBackend.h"
#include "Core/GDI/GfxQueue.h"
#include "Core/GDI/GfxDevice.h"
#include "Core/GDI/inc_gfx_types.h"
#include <d3d12.h>
#include "Core/GDI/d3dx12.h"
#include "Application.h"
#include "inc_core.h"
#include "Core/GDI/GfxCmdList.h"
#include "UI/UIEditorEvents.h"
#include "ResourceSystem/ResourceUtils.h"


namespace Dawn
{

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, (size_t)(&((VertexPosColor*)0)->Position), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  (size_t)(&((VertexPosColor*)0)->Color), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> psBlob;

	Mesh* usedMesh;

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


	void TestRenderLayer::Setup()
	{
		CEventDispatcher::Subscribe(FOVChangedEvtKey, BIND_EVENT_MEMBER(TestRenderLayer::OnFOVChanged));
		CEventDispatcher::Subscribe(CamPosChangedEvtKey, BIND_EVENT_MEMBER(TestRenderLayer::OnCamPosChanged));

		ComPtr<ID3D12Device2> Device = GfxBackend::GetDevice()->GetD3D12Device();
		auto CommandQueue = GfxBackend::GetQueue(D3D12_COMMAND_LIST_TYPE_COPY);
		auto GfxCmdList = CommandQueue->GetCommandList();
		CommandQueue->Flush();

		RefPtr<ResourceSystem> rs(ResourceSystem::Get());
		auto handle = rs->LoadFile(CREATE_FILE_HANDLE("Model/cornell_box.obj"));
		if (handle.IsValid)
		{
			usedMesh = ResourceSystem::GetMesh(handle);
			CopyMeshesToGPU(*GfxCmdList, { &usedMesh }, 1);
		}

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
		desc.DepthStencilState.DepthEnable = true;
		desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS;
		desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK::D3D12_DEPTH_WRITE_MASK_ALL;
		desc.DepthStencilState.StencilEnable = false;
		desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		//desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		desc.SampleDesc.Count = 1;
		desc.SampleMask = UINT_MAX;

		ThrowIfFailed(Device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&PipelineState)));

		auto fenceValue = CommandQueue->ExecuteCommandList(GfxCmdList);
		CommandQueue->WaitForFenceValue(fenceValue);
		CommandQueue->Flush();
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

	void TestRenderLayer::Render(ComPtr<ID3D12GraphicsCommandList2> InCmdList)
	{
		CGfxState* state = PipelineState.Get();

		
		auto rtv = GfxBackend::GetCurrentBackbufferDescHandle();
		auto dsv = GfxBackend::GetDepthBufferDescHandle();

		if (usedMesh != nullptr)
		{
			InCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			InCmdList->IASetVertexBuffers(0, 1, &usedMesh->VertexBufferView.GetVertexBufferView());
			InCmdList->IASetIndexBuffer(&usedMesh->IndexBufferView.GetIndexBufferView());
		}

		InCmdList->RSSetViewports(1, &GfxBackend::GetDevice()->GetViewport());
		InCmdList->RSSetScissorRects(1, &GfxBackend::GetDevice()->GetScissorRect());

		InCmdList->SetPipelineState(state);
		InCmdList->SetGraphicsRootSignature(RootSignature.Get());


		if (usedMesh != nullptr)
		{
			DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(ModelMatrix, ViewMatrix);
			mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, ProjectionMatrix);
			InCmdList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &mvpMatrix, 0);
			InCmdList->DrawIndexedInstanced(usedMesh->NumIndices, 1, 0, 0, 0);
		}
	}

	void TestRenderLayer::Free()
	{
		this->PipelineState.Reset();
		this->GfxIndexBuffer.Reset();
		this->VertexBuffer.Reset();
		this->RootSignature.Reset();
	}
}