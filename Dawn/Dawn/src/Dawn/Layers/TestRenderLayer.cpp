#include "TestRenderLayer.h"
#include "Core/GDI/inc_gfx.h"
#include "Application.h"
#include "inc_core.h"
#include "UI/UIEditorEvents.h"
#include "ResourceSystem/ResourceUtils.h"
#include "Rendering/RenderResourceHelper.h"
#include "EntitySystem/Camera/Camera.h"
#include "EntitySystem/Transform/Transform.h"
#include "Rendering/RenderDebugInterface.h"

namespace Dawn
{
	std::shared_ptr<Shader> pixelShader;
	std::shared_ptr<Shader> vertexShader;
	std::shared_ptr<Mesh> usedMesh;
	std::shared_ptr<Image> diffuseTexture;

	RefPtr<World> g_World;
	Camera* g_camera;
	Transform* g_camTransform;

	void TestRenderLayer::Setup()
	{
		RefPtr<ResourceSystem> rs(ResourceSystem::Get());

		// mesh loading
		{
			auto handle = rs->LoadFile("Model/cube.obj");
			if (handle.IsValid)
			{
				usedMesh = ResourceTable::GetMesh(handle);
				auto meshPtr = usedMesh.get();
				CopyMeshesToGPU({ &meshPtr }, 1);
			}
		}

		auto imageId = rs->LoadFile("Textures/crate0_diffuse.PNG");
		if (imageId.IsValid)
		{
			diffuseTexture = ResourceTable::GetImage(imageId);
			auto imagePtr = diffuseTexture.get();
			CopyImagesToGPU({ &imagePtr }, 1, { GL_REPEAT, GL_REPEAT }, { GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR });
		}

		g_World = World::Get();

		g_camera = g_World->GetCamera(0);
		g_camTransform = g_camera->GetTransform();
		CameraUtils::CalculateView(g_camera, g_camTransform);
		Debug::AllocateResources();

		auto components = g_World->GetComponentTypesByEntity(g_camera->Id.Entity);
		for (auto component : components)
			DWN_CORE_INFO("Component {0}", component);

	}

	vec3 tempTarget = vec3(0, 0, -1);
	float lastX, lastY = 0.0f;
	bool firstMouse = true;
	float yaw, pitch = 0.0f;

	void TestRenderLayer::Update()
	{
		// This is pretty wasteful for performance since we get the camera and transform 
		// each frame!
		auto time = static_cast<float>(Timer::GetTime().GetTotalSeconds());
		auto timeDelta = static_cast<float>(Timer::GetTime().GetDeltaSeconds());
		float angle = time * 90.0f;
		vec3 rotationAxis(0, 1, 0);
		mat4 trans(1.0f);
		Model = trans;//glm::rotate(trans, glm::radians(angle), rotationAxis);
	
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
			static float sensitivity = 0.5f;
			yaw += xoffset * sensitivity * timeDelta;
			pitch += yoffset * sensitivity * timeDelta;
			pitch = glm::clamp(pitch, -90.0f, 90.0f);

			glm::quat qPitch = glm::angleAxis(pitch, glm::vec3(1, 0, 0));
			glm::quat qYaw = glm::angleAxis(yaw, glm::vec3(0, 1, 0));
			g_camTransform->Rotation = glm::normalize(qPitch * qYaw);

			vec3 right = TransformUtils::CalculateRight(g_camTransform);
			vec3 up = TransformUtils::CalculateUp(g_camTransform);
			vec3 forward = TransformUtils::CalculateForward(g_camTransform);

			float velocity = (Input::IsKeyDown(KeyCode_Shift)) ? 25.5f : 6.5f;


			if (Input::IsKeyDown(KeyCode_A))
			{
				g_camTransform->Position +=  right *  velocity * timeDelta;
			}

			if (Input::IsKeyDown(KeyCode_D))
			{
				g_camTransform->Position -= right *  velocity * timeDelta;
			}

			if (Input::IsKeyDown(KeyCode_W))
			{
				g_camTransform->Position += forward * velocity * timeDelta;
			}

			if (Input::IsKeyDown(KeyCode_S))
			{
				g_camTransform->Position -= forward * velocity * timeDelta;
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

		CameraUtils::CalculateView(g_camera, g_camTransform);

		// render transform cross
		{
			// Z Axis
			Ray r = { vec3(0,0,0), vec3(0,0,1) };
			vec4 color = { 0, 0, 1, 1 };
			Debug::DrawRay(g_camera, r, 1.0f, color);
			// Y Axis
			r = { vec3(0,0,0), vec3(0,1,0) };
			color = { 0, 1, 0, 1 };
			Debug::DrawRay(g_camera, r, 1.0f, color);

			// Y Axis
			r = { vec3(0,0,0), vec3(1,0,0) };
			color = { 1, 0, 0, 1 };
			Debug::DrawRay(g_camera, r, 1.0f, color);
		}
	}

	void TestRenderLayer::Render()
	{
		BROFILER_CATEGORY("RenderLayer_Render", Brofiler::Color::AliceBlue);

		//if (!RenderResourceHelper::CommonShaders.ID_Debug.IsValid)
		//	return;
		
		auto shader = ResourceTable::GetShader(CommonShaderHandles::Debug);
		glUseProgram(shader->GDI_ShaderId);

		glActiveTexture(GL_TEXTURE0);

		glUniform1i(glGetUniformLocation(shader->GDI_ShaderId, "ourTexture"), 0);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture->GDI_TextureId);

		// set pojection
		glUniformMatrix4fv(glGetUniformLocation(shader->GDI_ShaderId, "model"), 1, GL_FALSE, &Model[0][0]);

		// set pojection
		glUniformMatrix4fv(glGetUniformLocation(shader->GDI_ShaderId, "view"), 1, GL_FALSE, &g_camera->GetView()[0][0]);

		// set pojection
		glUniformMatrix4fv(glGetUniformLocation(shader->GDI_ShaderId, "projection"), 1, GL_FALSE, &g_camera->GetProjection()[0][0]);

		glBindVertexArray(usedMesh->GDI_VAOId);
		glDrawElements(GL_TRIANGLES, usedMesh->NumIndices, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);

		vec3 pos(0, 0, 0);
		// refactor this!
		Debug::Quad(pos, g_camera);

	}

	void TestRenderLayer::Free()
	{
		glDeleteVertexArrays(1, &usedMesh->GDI_VAOId);

		/*diffuseTexture->TextureView.Reset();
		diffuseTexture.reset();

		usedMesh->IndexBufferView.Reset();
		usedMesh->VertexBufferView.Reset();
		usedMesh.reset();*/
	}
}