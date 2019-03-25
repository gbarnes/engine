#include "TestRenderLayer.h"
#include "Core/GDI/GfxBackend.h"
#include "Core/GDI/GfxQueue.h"
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
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	D3D12_INPUT_ELEMENT_DESC inputLayout2[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,  D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};


	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> psBlob;

	std::shared_ptr<Shader> pixelShader;
	std::shared_ptr<Shader> vertexShader;
	std::shared_ptr<Mesh> usedMesh;
	std::shared_ptr<Image> diffuseTexture;

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

		ComPtr<ID3D12Device2> Device = GfxBackend::GetDevice();
		auto CommandQueue = GfxBackend::GetQueue(D3D12_COMMAND_LIST_TYPE_COPY);
		auto GfxCmdList = CommandQueue->GetCommandList();
		CommandQueue->Flush();

		RefPtr<ResourceSystem> rs(ResourceSystem::Get());

		// mesh loading
		{
			auto handle = rs->LoadFile("Model/cube.obj");
			if (handle.IsValid)
			{
				usedMesh = ResourceTable::GetMesh(handle);
				auto meshPtr = usedMesh.get();
				CopyMeshesToGPU(*GfxCmdList, { &meshPtr }, 1);
			}
		}

		// vertex shader loading
		auto shaderId = rs->LoadFile("Shader/phong_vs.cso");
		if (shaderId.IsValid)
		{
			vertexShader = ResourceTable::GetShader(shaderId);
		}

		// pixel shader loading
		shaderId = rs->LoadFile("Shader/phong_ps.cso");
		if (shaderId.IsValid)
		{
			pixelShader = ResourceTable::GetShader(shaderId);
		}

		auto imageId = rs->LoadFile("Textures/crate0_diffuse.png");
		if (imageId.IsValid)
		{
			diffuseTexture = ResourceTable::GetImage(imageId);
			auto imagePtr = diffuseTexture.get();
			CopyImagesToGPU(*GfxCmdList, { &imagePtr }, 1);
		}

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
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

		// A single 32-bit constant root parameter that is used by the vertex shader.
		CD3DX12_ROOT_PARAMETER1 rootParameters[2];
		rootParameters[0].InitAsConstants(sizeof(DirectX::XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

		CD3DX12_DESCRIPTOR_RANGE1 textureSRV(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
		CD3DX12_DESCRIPTOR_RANGE1 textureSampler(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

		rootParameters[1].InitAsDescriptorTable(1, &textureSRV, D3D12_SHADER_VISIBILITY_PIXEL);


		CD3DX12_STATIC_SAMPLER_DESC linearRepeatSampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
		CD3DX12_STATIC_SAMPLER_DESC anisotropic(0, D3D12_FILTER_COMPARISON_ANISOTROPIC);


		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
		rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 1, &anisotropic, rootSignatureFlags);

		RootSignature.SetRootSignatureDesc(rootSignatureDescription.Desc_1_1, featureData.HighestVersion);
		
		// Serialize the root signature.
		/*ComPtr<ID3DBlob> rootSignatureBlob;
		ComPtr<ID3DBlob> errorBlob;
		ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
			featureData.HighestVersion, &rootSignatureBlob, &errorBlob));

		// Create the root signature.
		ThrowIfFailed(Device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
			rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&RootSig)));*/


		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature = RootSignature.GetRootSignature().Get();
		desc.InputLayout = { inputLayout2, _countof(inputLayout2) };
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.VS = CD3DX12_SHADER_BYTECODE(vertexShader->D3DData.Get());
		desc.PS = CD3DX12_SHADER_BYTECODE(pixelShader->D3DData.Get());
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
		//desc.SampleDesc = GfxBackend::GetMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, D3D12_MAX_MULTISAMPLE_SAMPLE_COUNT);
		//desc.SampleDesc.Count = 4;
		//desc.SampleDesc.Quality = DXGI_STANDARD_MULTISAMPLE_QUALITY_PATTERN;

		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
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

	void TestRenderLayer::Render(GfxCmdList* InCmdList)
	{
		BROFILER_CATEGORY("RenderLayer_Render", Brofiler::Color::AliceBlue)
		CGfxState* state = PipelineState.Get();
		auto innerCmdList = InCmdList->GetGraphicsCommandList();

		InCmdList->SetPipelineState(state);
		InCmdList->SetGraphicsRootSignature(RootSignature);
		

		if (usedMesh != nullptr)
		{
			InCmdList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			InCmdList->SetVertexBuffer(0, usedMesh->VertexBufferView);
			InCmdList->SetIndexBuffer(usedMesh->IndexBufferView);
		}


		if (usedMesh != nullptr)
		{
			DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(ModelMatrix, ViewMatrix);
			mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, ProjectionMatrix);

			InCmdList->SetGraphics32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &mvpMatrix);
			InCmdList->SetShaderResourceView(1, 0, diffuseTexture->TextureView, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

			InCmdList->DrawIndexed(usedMesh->NumIndices);
		}
	}

	void TestRenderLayer::Free()
	{
		diffuseTexture->TextureView.Reset();
		diffuseTexture.reset();

		pixelShader->D3DData.Reset();
		pixelShader.reset();

		vertexShader->D3DData.Reset();
		vertexShader.reset();

		usedMesh->IndexBufferView.Reset();
		usedMesh->VertexBufferView.Reset();
		usedMesh.reset();

		this->PipelineState.Reset();
		this->GfxIndexBuffer.Reset();
		this->VertexBuffer.Reset();
		this->RootSignature.GetRootSignature().Reset();
	}
}