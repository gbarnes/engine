#include "RenderDebugInterface.h"
#include "Core/GDI/inc_gfx.h"
#include "RenderResourceHelper.h"
#include <windows.h>
#include <gl\GL.h>
#include "ResourceSystem/Resources.h"
#include "ResourceSystem/ResourceSystem.h"
#include "ResourceSystem/ResourceUtils.h"
#include "EntitySystem/Camera/Camera.h"

namespace Dawn
{
	/*VertexPosUV1 vertices[] = {
		{vec3(-0.5f,  0.5f, 0.0f), vec2(0.0f, 0.0f) },
		{vec3(0.5f,  0.5f,  0.0f), vec2(1000.0f, 0.0f) },
		{vec3(0.5f, -0.5f,  0.0f), vec2(1000.0f, 1000.0f) },
		{vec3(-0.5f, -0.5f, 0.0f), vec2(0.0f, 1000.0f) },
	};*/

	float vertices[4*5] = {
		-0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
		0.5f,  0.5f,  0.0f, 1000.0f, 0.0f,
		0.5f, -0.5f,  0.0f, 1000.0f, 1000.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 1000.0f
	};

	u16 indices[6] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	u32 RAYVBO, RAYVAO;
	std::shared_ptr<Image> GridImage;
	GfxResId GridVertexArrayId, RayVertexArrayId;
	

	void Debug::AllocateResources()
	{
		// Grid Vertex Objects
		{
			GfxVertexArray* GridArray;
			GridVertexArrayId = GfxGDI::Get()->CreateVertexArray(&GridArray);

			GfxBufferLayout Layout = 
			{
				{ GfxShaderDataType::Float3, "aPos" },
				{ GfxShaderDataType::Float2, "aUV0" }
			};

			GfxVertexBuffer* VertexBuffer;

			GfxGDI::Get()->CreateVertexBuffer(vertices, sizeof(vertices), &VertexBuffer);
			VertexBuffer->SetLayout(Layout);
			GridArray->AttachVertexBuffer(VertexBuffer);

			GfxIndexBuffer* IndexBuffer;
			GfxGDI::Get()->CreateIndexBuffer(indices, sizeof(indices) / sizeof(u16), &IndexBuffer);

			GridArray->SetIndexBuffer(IndexBuffer);
		}

		// RAY VERTEX OBJECTS
		{
			glGenVertexArrays(1, &RAYVAO);
			glGenBuffers(1, &RAYVBO);

			glBindVertexArray(RAYVAO);
			glBindBuffer(GL_ARRAY_BUFFER, RAYVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPos) * 2, nullptr, GL_STATIC_DRAW);

			// vertex positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPos), (void*)0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		auto rs = ResourceSystem::Get();
		auto imageId = rs->LoadFile("Textures/grid.png");
		if (imageId.IsValid)
		{
			GridImage = ResourceTable::GetImage(imageId);
			auto imagePtr = GridImage.get();
			imagePtr->TextureId = GfxGDI::Get()->CreateTexture(imagePtr->Pixels,
				imagePtr->Width,
				imagePtr->Height,
				imagePtr->ChannelsPerPixel,
				{ GL_REPEAT, GL_REPEAT }, { GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR }, true, nullptr);
			//CopyImagesToGPU({ &imagePtr }, 1, { GL_REPEAT, GL_REPEAT }, { GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR }, true);
		}
	}

	void Debug::Box()
	{

	}

	void Debug::Line()
	{

	}

	void Debug::Quad(vec3& InPosition, Camera* InCamera)
	{
		auto& shaderHandle = EditorShaderHandles::Grid;
		auto shader = ResourceTable::GetShader(shaderHandle);
		auto gdiShader = shader->GetResource();

		mat4 trans(1);
		trans = glm::translate(trans, InPosition) * glm::rotate(trans, glm::radians(90.f), vec3(1, 0, 0)) * glm::scale(trans, vec3(1000, 1000, 1000));
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		gdiShader->Bind();

		glActiveTexture(GL_TEXTURE0);
		gdiShader->SetInt("gridTexture", 0);
		GridImage->GetResource()->Bind();

		// set pojection
		gdiShader->SetMat4("model", trans);
		gdiShader->SetMat4("view", InCamera->GetView());
		gdiShader->SetMat4("projection", InCamera->GetProjection());
		GfxGDI::Get()->DrawIndexed(GridVertexArrayId);

		GridImage->GetResource()->Unbind();
		gdiShader->Unbind();
		glDisable(GL_BLEND);
	}

	void Debug::Plane()
	{
		
		/*glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex);
		
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
		glTexCoord2f(1024, 0); glVertex3f(1, 0, 0);
		glTexCoord2f(1024, 1024); glVertex3f(1, 0, 1);
		glTexCoord2f(0, 1024); glVertex3f(0, 0, 1);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);*/
		
	}

	void Debug::DrawRay(Camera* InCamera, const Ray& InRay, float InLength, const vec4& InColor)
	{
		auto& shaderHandle = CommonShaderHandles::DebugPrim;
		auto shader = ResourceTable::GetShader(shaderHandle);
		auto gdiShader = shader->GetResource();
		mat4 trans(1);

		VertexPos ray[] = {
			{InRay.Pos},
			{InRay.Pos + (InRay.Dir) * InLength}
		};

		u32 vbo = 0;
		
		gdiShader->Bind();
		// set pojection
		gdiShader->SetMat4("model", trans);
		gdiShader->SetMat4("view", InCamera->GetView());
		gdiShader->SetMat4("projection", InCamera->GetProjection());
		gdiShader->SetVec4("myColor", InColor);

		glBindVertexArray(RAYVAO);
		glBindBuffer(GL_ARRAY_BUFFER, RAYVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ray), ray, GL_STATIC_DRAW);

		glDrawArrays(GL_LINE_STRIP, 0, 2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		gdiShader->Unbind();
	}


}