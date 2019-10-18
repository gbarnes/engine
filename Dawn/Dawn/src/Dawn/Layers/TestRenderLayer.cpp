#include "TestRenderLayer.h"
#include "Core/GDI/inc_gfx.h"
#include "Rendering/Renderer.h"
#include "Application.h"
#include "inc_core.h"
#include "UI/UIEditorEvents.h"
#include "ResourceSystem/ResourceUtils.h"
#include "Rendering/RenderResourceHelper.h"
#include "EntitySystem/Camera/Camera.h"
#include "EntitySystem/Transform/Transform.h"
#include "EntitySystem/Lights/LightComponents.h"
#include "EntitySystem/Entity.h"
#include "ResourceSystem/Resources.h"
#include "imgui.h"
#include "JobSystem/JobSystem.h"

namespace Dawn
{
	Shader* pixelShader;
	Shader* vertexShader;
	Model* usedModel;
	Image* DiffuseImage;
	Material* SampleMaterial;
	GfxVertexArray* CubeMeshArray;
	GfxVertexBuffer* ModelMatrixBuffer;

	Shared<World> g_World;
	Camera* g_camera;
	Camera* g_camera1;
	Transform* g_camTransform;

	GfxResId FinalPassQuadId;
	EntityId DirectionalLightId;

	vec3 tempTarget = vec3(0, 0, -1);
	float lastX, lastY = 0.0f;
	bool firstMouse = true;
	float yaw = 0.0f; 
	float pitch = 0.0f;
	vec3 right = vec3(1.0f, 0.0f, 0.0f), forward = vec3(0.0f, 0.0f, -1.0f), up = vec3(0.0f, 1.0f, 0.0f);
	u64 CubeDrawKey;
	constexpr u32 Size = 10 * 10;

	TestRenderLayer::TestRenderLayer(Shared<Dawn::Application> InApplication)
		: Layer(InApplication)
	{

	}

	void TestRenderLayer::Setup()
	{
		const auto GDI = Application->GetGDI();
		const auto World = Application->GetWorld();
		const auto RS = Application->GetResourceSystem();

		// mesh loading
		{
			auto handle = RS->LoadFile("Model/spaceCraft1.fbx");
			if (handle.IsValid)
			{
				usedModel = RS->FindModel(handle);
			}
		}

		auto imageId = RS->LoadFile("Textures/smiley.jpg");
		if (imageId.IsValid)
		{
			DiffuseImage = RS->FindImage(imageId);
		}

		g_World = World;

		g_camera = g_World->GetCamera(0);
		g_camTransform = g_camera->GetTransform(World.get());
		CameraUtils::CalculateView(g_camera, g_camTransform);

		g_camera1 = g_World->GetCamera(1);
		CameraUtils::CalculateView(g_camera1, g_camera1->GetTransform(World.get()));

		auto Light = LightUtils::CreateDirectionalLight(World.get(), quat(), vec4(0.9, 0.9, 0.9, 1.0f));
		DirectionalLightId = Light->Id.Entity;

		auto Quad = GfxPrimitiveFactory::AllocateQuad(GDI.get(), vec2(1.0f, -1.0f), 1.0f);
		FinalPassQuadId = Quad->GetId();

		
		auto MatId = RS->CreateMaterial(&SampleMaterial);
		SampleMaterial->DiffuseColor = vec4(0.7f, 0.7f, 0.7f, 1.0f);
		SampleMaterial->ShaderId = CommonShaderHandles::StandardInstanced;



		{
			auto handle = RS->LoadFile("Model/cube.obj");
			if (handle.IsValid)
			{
				auto cube = RS->FindModel(handle);
				auto mesh = RS->FindMesh(cube->Meshes[0]);

				glm::mat4* modelMatrices = new glm::mat4[Size];
				u32 row = 0;
				u32 columnCount = 0;
				u32 index = 0;
				for (u32 i = 0; i < 10; ++i)
				{
					for (u32 y = 0; y < 10; ++y)
					{
						glm::mat4 model = glm::mat4(1.0f);
						model = glm::translate(model, vec3(i * 2.1f, -1.0f, y * 2.1f));
						model = glm::scale(model, glm::vec3(2.0f));
						modelMatrices[index] = (model);
						++index;
					}
				}

				CubeMeshArray = GDI->GetVertexArray(mesh->VertexArrayId);

				GfxBufferLayout Layout =
				{
					{ GfxShaderDataType::Float4, "instanceMatrix" },
					{ GfxShaderDataType::Float4, "instanceMatrix" },
					{ GfxShaderDataType::Float4, "instanceMatrix" },
					{ GfxShaderDataType::Float4, "instanceMatrix" }
				};

				GDI->CreateVertexBuffer(&modelMatrices[0], Size * sizeof(glm::mat4), &ModelMatrixBuffer);
				ModelMatrixBuffer->SetLayout(Layout);
				CubeMeshArray->AttachVertexBuffer(ModelMatrixBuffer, 1);
				delete[] modelMatrices;
			}
		}

		CubeDrawKey = GenDrawKey64(true, SampleMaterial->Id.Index, RenderLayer::StaticGeom, 0.0f);
	}

	void TestRenderLayer::Update(float InDeltaTime)
	{
		Model = glm::scale(mat4(1), vec3(0.1f));//glm::translate(mat4(1), vec3(-2.0f, 0.0f, 0.0f)) * glm::mat4_cast(rotation);
	
		vec2 mousePosition = Input::GetMousePosition();

		if (firstMouse)
		{
			lastX = mousePosition.x;
			lastY = mousePosition.y;
			firstMouse = false;
		}

		float xoffset = mousePosition.x - lastX;
		float yoffset = lastY - mousePosition.y;

		lastX = mousePosition.x;
		lastY = mousePosition.y;

		
		if (Input::IsMouseButtonDown(MouseBtn_Right))
		{
			static float sensitivity = 30.0f;
			yaw += xoffset * sensitivity * InDeltaTime;
			pitch += yoffset * sensitivity *InDeltaTime;

			up = TransformUtils::CalculateUp(g_camTransform);
			forward = TransformUtils::CalculateForward(g_camTransform);
			right = TransformUtils::CalculateRight(g_camTransform);

			float velocity = (Input::IsKeyDown(KeyCode_Shift)) ? 15.5f : 10.0f;


			if (Input::IsKeyDown(KeyCode_A))
			{
				g_camTransform->Position -=  right *  velocity * InDeltaTime;
			}

			if (Input::IsKeyDown(KeyCode_D))
			{
				g_camTransform->Position += right *  velocity * InDeltaTime;
			}

			if (Input::IsKeyDown(KeyCode_W))
			{
				g_camTransform->Position -= forward * velocity * InDeltaTime;
			}

			if (Input::IsKeyDown(KeyCode_S))
			{
				g_camTransform->Position += forward * velocity * InDeltaTime;
			}
			
			if (Input::IsKeyDown(KeyCode_Q))
			{
				g_camTransform->Position += g_camera->WorldUp * velocity * InDeltaTime;
			}

			if (Input::IsKeyDown(KeyCode_E))
			{
				g_camTransform->Position -= g_camera->WorldUp * velocity * InDeltaTime;
			}
		}
		
		quat rot = glm::angleAxis(glm::radians(-yaw), vec3(0, 1, 0)) * glm::angleAxis(glm::radians(pitch), vec3(1, 0, 0));
		TransformUtils::Rotate(g_camTransform, rot);
	}


	void TestRenderLayer::Render()
	{
		BROFILER_CATEGORY("RenderLayer_Render", Brofiler::Color::AliceBlue);

		CameraUtils::CalculateView(g_camera, g_camTransform);
		CameraUtils::CalculateView(g_camera1, g_camera1->GetTransform(g_World.get()));

		auto Renderer = Application->GetRenderer();
		auto GDI = Application->GetGDI();

		auto ViewportCmd = Renderer->PerFrameData.GeometryBucket.AddCommand<Draw::SetViewportData>(0u);
		ViewportCmd->Width = g_camera->Width;
		ViewportCmd->Height = g_camera->Height;

		auto ClearData = Renderer->PerFrameData.GeometryBucket.AppendCommand<Draw::ClearSceneData>(ViewportCmd);
		ClearData->ClearColor = g_camera->ClearColor;

		auto ClearFinalPass = Renderer->PerFrameData.FinalPassBucket.AddCommand<Draw::ClearSceneData>(0u);
		ClearFinalPass->ClearColor = g_camera->ClearColor;

		auto FinalPassData = Renderer->PerFrameData.FinalPassBucket.AppendCommand<Draw::FinalPassCombineData>(ClearFinalPass);
		FinalPassData->ShaderId = CommonShaderHandles::FinalPass;
		FinalPassData->RenderBufferId = Renderer->TransientData.RenderBufferId;
		FinalPassData->FSQuadVAOId = FinalPassQuadId;

		const auto ResourceSystem = Application->GetResourceSystem();
		if (usedModel != nullptr && CommonShaderHandles::Standard.IsValid)
		{
			for (const auto &id : usedModel->Meshes)
			{
				const auto Mesh = ResourceSystem->FindMesh(id);
				if (Mesh)
				{
					for (const auto& MaterialId : Mesh->Materials)
					{
						auto Key = GenDrawKey64(true, MaterialId.Index, RenderLayer::StaticGeom, 0.0f);
						auto DrawCmd = Renderer->PerFrameData.GeometryBucket.AddCommand<Draw::DrawIndexedData>(Key);
						DrawCmd->IndexCount = Mesh->NumIndices;
						DrawCmd->VertexArrayId = Mesh->VertexArrayId;
						DrawCmd->Model = Model;
					}
				}
			}
		}


		//JobSystem::CreateJob();
		auto DrawCmd = Renderer->PerFrameData.GeometryBucket.AddCommand<Draw::DrawInstancedData>(CubeDrawKey);
		DrawCmd->VertexArrayId = CubeMeshArray->GetId();
		DrawCmd->IndexCount = CubeMeshArray->GetIndexBuffer(GDI.get())->GetSize();
		DrawCmd->Amount = Size;
		/*
		Primitives->SetCamera(g_camera);
		Primitives->Grid(vec3(0, 0, 0), vec3(1000, 1000, 1000));

		Primitives->SetCamera(g_camera1);
		Primitives->Quad(GDI->GetTexture(DiffuseImage->TextureId), vec3(100, g_camera->Height - 100.0f, 0), vec3(100.0f));
		Primitives->Axis(vec3(100, 50, -100), vec3(75), g_camTransform->Rotation);*/
	}

	void TestRenderLayer::Free()
	{
	}
}