#include "TestRenderLayer.h"
#include "Core/GDI/inc_gfx.h"
#include "Application.h"
#include "inc_core.h"
#include "UI/UIEditorEvents.h"
#include "ResourceSystem/ResourceUtils.h"
#include "Rendering/RenderResourceHelper.h"
#include "EntitySystem/Camera/Camera.h"
#include "EntitySystem/Transform/Transform.h"
#include "imgui.h"

namespace Dawn
{
	std::shared_ptr<Shader> pixelShader;
	std::shared_ptr<Shader> vertexShader;
	std::shared_ptr<Model> usedModel;
	std::shared_ptr<Image> diffuseTexture;

	RefPtr<World> g_World;
	Camera* g_camera;
	Camera* g_camera1;
	Transform* g_camTransform;

	vec3 tempTarget = vec3(0, 0, -1);
	float lastX, lastY = 0.0f;
	bool firstMouse = true;
	float yaw = 0.0f; 
	float pitch = 0.0f;
	vec3 right = vec3(1.0f, 0.0f, 0.0f), forward = vec3(0.0f, 0.0f, -1.0f), up = vec3(0.0f, 1.0f, 0.0f);

	void TestRenderLayer::Setup()
	{
		auto GDI = GfxGDI::Get();
		RefPtr<ResourceSystem> rs(ResourceSystem::Get());

		// mesh loading
		{
			auto handle = rs->LoadFile("Model/spaceCraft1.fbx");
			if (handle.IsValid)
			{
				usedModel = ResourceTable::GetModel(handle);
			}
		}

		auto imageId = rs->LoadFile("Textures/crate0_diffuse.PNG");
		if (imageId.IsValid)
		{
			diffuseTexture = ResourceTable::GetImage(imageId);
			auto imagePtr = diffuseTexture.get();

			// note (gb): I'm not sure yet if the texture id should be connected to the image
			imagePtr->TextureId = GDI->CreateTexture(imagePtr->Pixels,
				imagePtr->Width, 
				imagePtr->Height, 
				imagePtr->ChannelsPerPixel, 
				{ GL_REPEAT, GL_REPEAT }, { GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR }, true, nullptr);

			//CopyImagesToGPU({ &imagePtr }, 1, { GL_REPEAT, GL_REPEAT }, { GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR });
		}

		g_World = World::Get();

		g_camera = g_World->GetCamera(0);
		g_camTransform = g_camera->GetTransform();
		CameraUtils::CalculateView(g_camera, g_camTransform);

		g_camera1 = g_World->GetCamera(1);
		CameraUtils::CalculateView(g_camera1, g_camera1->GetTransform());
	}

	quat rotation;
	float pitch2 = 0;
	void TestRenderLayer::Update()
	{
		// This is pretty wasteful for performance since we get the camera and transform 
		// each frame! 
		auto time = static_cast<float>(Timer::GetTime().GetTotalSeconds());
		auto timeDelta = static_cast<float>(Timer::GetTime().GetDeltaSeconds());
		float angle = time * 90.0f;
		vec3 rotationAxis(0, 1, 0);
		
		pitch2 += 20.0f * timeDelta;

		rotation = glm::angleAxis(glm::radians(pitch2), vec3(0, 1, 0));
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
			static float sensitivity = 12.0f;
			yaw += xoffset * sensitivity * timeDelta;
			pitch += yoffset * sensitivity * timeDelta;

			up = TransformUtils::CalculateUp(g_camTransform);
			forward = TransformUtils::CalculateForward(g_camTransform);
			right = TransformUtils::CalculateRight(g_camTransform);

			float velocity = (Input::IsKeyDown(KeyCode_Shift)) ? 25.5f : 6.5f;


			if (Input::IsKeyDown(KeyCode_A))
			{
				g_camTransform->Position -=  right *  velocity * timeDelta;
			}

			if (Input::IsKeyDown(KeyCode_D))
			{
				g_camTransform->Position += right *  velocity * timeDelta;
			}

			if (Input::IsKeyDown(KeyCode_W))
			{
				g_camTransform->Position -= forward * velocity * timeDelta;
			}

			if (Input::IsKeyDown(KeyCode_S))
			{
				g_camTransform->Position += forward * velocity * timeDelta;
			}
			
			if (Input::IsKeyDown(KeyCode_Q))
			{
				g_camTransform->Position += g_camera->WorldUp * velocity * timeDelta;
			}

			if (Input::IsKeyDown(KeyCode_E))
			{
				g_camTransform->Position -= g_camera->WorldUp * velocity * timeDelta;
			}
		}
		
		quat rot = glm::angleAxis(glm::radians(-yaw), vec3(0, 1, 0)) * glm::angleAxis(glm::radians(pitch), vec3(1, 0, 0));
		TransformUtils::Rotate(g_camTransform, rot);

		CameraUtils::CalculateView(g_camera, g_camTransform);
		CameraUtils::CalculateView(g_camera1, g_camera1->GetTransform());
	}

	void TestRenderLayer::Render()
	{
		BROFILER_CATEGORY("RenderLayer_Render", Brofiler::Color::AliceBlue);

		if (usedModel == nullptr)
			return;

		if (!CommonShaderHandles::Debug.IsValid)
			return;

		
		//glCullFace(GL_BACK);
		//glFrontFace(GL_CW);

		auto shader = ResourceTable::GetShader(CommonShaderHandles::Debug);
		assert(shader != nullptr);

		auto shaderRes = shader->GetResource();
		if (shaderRes)
		{
			shaderRes->Bind();

			// set pojection
			shaderRes->SetMat4("model", Model);
			shaderRes->SetMat4("view", g_camera->GetView());
			shaderRes->SetMat4("projection", g_camera->GetProjection());
			
			glActiveTexture(GL_TEXTURE0);
			shaderRes->SetInt("ourTexture", 0);

			for (MeshHandle id : usedModel->Meshes)
			{
				diffuseTexture->GetResource()->Bind();

				auto mesh = ResourceTable::GetMesh(id);
				if(mesh)
					GfxGDI::Get()->DrawIndexed(mesh->VertexArrayId);


				diffuseTexture->GetResource()->Unbind();
			}


			shaderRes->Unbind();
		}


		DrawGrid(vec3(0,0,0), vec3(1000, 1000, 1000), g_camera);
		DrawAxis(g_camera1, vec3(100, g_camera->Height - 100.0f, -100), vec3(75), g_camera->GetTransform()->Rotation);
	}

	void TestRenderLayer::Process()
	{
		Update();
		Render();
	}

	void TestRenderLayer::Free()
	{
		//glDeleteVertexArrays(1, &usedMesh->GDI_VAOId);

		/*diffuseTexture->TextureView.Reset();
		diffuseTexture.reset();

		usedMesh->IndexBufferView.Reset();
		usedMesh->VertexBufferView.Reset();
		usedMesh.reset();*/
	}
}