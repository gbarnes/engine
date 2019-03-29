#include "TestRenderLayer.h"
#include "Core/GDI/inc_gfx.h"
#include "Application.h"
#include "inc_core.h"
#include "UI/UIEditorEvents.h"
#include "ResourceSystem/ResourceUtils.h"
#include "Rendering/RenderResourceHelper.h"

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

		auto imageId = rs->LoadFile("Textures/crate0_diffuse.png");
		if (imageId.IsValid)
		{
			diffuseTexture = ResourceTable::GetImage(imageId);
			auto imagePtr = diffuseTexture.get();
			CopyImagesToGPU({ &imagePtr }, 1);
		}
	}

	void TestRenderLayer::Update()
	{
		float angle = static_cast<float>(Timer::GetTime().GetTotalSeconds() * 90.0);
		vec3 rotationAxis(0, 1, 0);
		mat4 trans(1.0f);
		Model = glm::rotate(trans, glm::radians(angle), rotationAxis);

		vec3 eyePos(CamPosition[0], CamPosition[1], CamPosition[2]);
		vec3 focusPoint(0, 0, 0);
		vec3 upDirection(0, 1, 0);
		View = glm::lookAt(eyePos, focusPoint, upDirection);

		AppSettings* Settings = Application::GetSettings();
		Perspective = glm::perspective(glm::radians(this->FoV), (float)Settings->Width / (float)Settings->Height, 0.1f, 100.0f);
	}

	void TestRenderLayer::Render()
	{
		BROFILER_CATEGORY("RenderLayer_Render", Brofiler::Color::AliceBlue);

		//if (!RenderResourceHelper::CommonShaders.ID_Debug.IsValid)
		//	return;

		auto shader = ResourceTable::GetShader(RenderResourceHelper::CommonShaders.ID_Debug);
		glUseProgram(shader->GDI_ShaderId);

		glActiveTexture(GL_TEXTURE0);

		glUniform1i(glGetUniformLocation(shader->GDI_ShaderId, "ourTexture"), 0);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture->GDI_TextureId);

		// set pojection
		glUniformMatrix4fv(glGetUniformLocation(shader->GDI_ShaderId, "model"), 1, GL_FALSE, &Model[0][0]);

		// set pojection
		glUniformMatrix4fv(glGetUniformLocation(shader->GDI_ShaderId, "view"), 1, GL_FALSE, &View[0][0]);

		// set pojection
		glUniformMatrix4fv(glGetUniformLocation(shader->GDI_ShaderId, "projection"), 1, GL_FALSE, &Perspective[0][0]);


		glBindVertexArray(usedMesh->GDI_VAOId);
		glDrawElements(GL_TRIANGLES, usedMesh->NumIndices, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);
	}

	void TestRenderLayer::Free()
	{
		/*diffuseTexture->TextureView.Reset();
		diffuseTexture.reset();

		usedMesh->IndexBufferView.Reset();
		usedMesh->VertexBufferView.Reset();
		usedMesh.reset();*/
	}
}