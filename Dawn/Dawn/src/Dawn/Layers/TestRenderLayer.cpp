#include "TestRenderLayer.h"
#include "Core/GDI/GfxBackend.h"
#include "Core/GDI/GfxQueue.h"
#include "Core/GDI/inc_gfx_types.h"
#include "Application.h"
#include "inc_core.h"
#include "Core/GDI/GfxCmdList.h"
#include "UI/UIEditorEvents.h"
#include "ResourceSystem/ResourceUtils.h"
#include "debug_draw.h"

namespace Dawn
{
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

		auto imageId = rs->LoadFile("Textures/crate0_diffuse.png");
		if (imageId.IsValid)
		{
			diffuseTexture = ResourceTable::GetImage(imageId);
			auto imagePtr = diffuseTexture.get();
			CopyImagesToGPU(*GfxCmdList, { &imagePtr }, 1);
		}

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

		auto innerCmdList = InCmdList->GetGraphicsCommandList();

		InCmdList->SetPipelineState(GfxBackend::GetPipelineState(0));
		InCmdList->SetGraphicsRootSignature(*GfxBackend::GetRootSignature(0).get());
		
		if (usedMesh != nullptr)
		{
			DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(ModelMatrix, ViewMatrix);
			mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, ProjectionMatrix);
			
			InCmdList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			InCmdList->SetVertexBuffer(0, usedMesh->VertexBufferView);
			InCmdList->SetIndexBuffer(usedMesh->IndexBufferView);
			InCmdList->SetGraphics32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &mvpMatrix);
			InCmdList->SetShaderResourceView(1, 0, diffuseTexture->TextureView, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			InCmdList->DrawIndexed(usedMesh->NumIndices);
		}
	}

	void TestRenderLayer::Free()
	{
		diffuseTexture->TextureView.Reset();
		diffuseTexture.reset();

		usedMesh->IndexBufferView.Reset();
		usedMesh->VertexBufferView.Reset();
		usedMesh.reset();

		this->GfxIndexBuffer.Reset();
		this->VertexBuffer.Reset();
	}
}