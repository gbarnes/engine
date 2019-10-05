#include "GLImmediatePrimitives.h"
#include "../inc_gfx.h"
#include "glad.h"
#include "EntitySystem/Camera/Camera.h"
#include "ResourceSystem/ResourceSystem.h"
#include "Rendering/RenderResourceHelper.h"

namespace Dawn
{
	GfxResId GridVertexArrayId, RayVertexArrayId;
	GfxTexture* GridTexture;
	GfxShader* PrimitiveShader;
	GfxShader* GridShader;

	void GLImmediatePrimitives::AllocateBuffers()
	{
		auto GDI = GfxGDI::Get();

		// Textured-Grid Allocation
		{
			GridVertexArrayId = GfxPrimitiveFactory::AllocateGrid()->GetId();

			auto rs = ResourceSystem::Get();
			auto imageId = rs->LoadFile("Textures/grid.png");
			if (imageId.IsValid)
			{
				auto imageResource = ResourceTable::GetImage(imageId);
				if (imageResource)
				{
					GDI->CreateTexture
					(
						imageResource->Pixels,
						imageResource->Width,
						imageResource->Height,
						imageResource->ChannelsPerPixel,
						{ GL_REPEAT, GL_REPEAT }, { GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR },
						true,
						&GridTexture
					);
				}
			}
		}

		RayVertexArrayId = GfxPrimitiveFactory::AllocateLine()->GetId();
		PrimitiveShader = ResourceTable::GetShader(CommonShaderHandles::DebugPrim)->GetResource();
		GridShader = ResourceTable::GetShader(EditorShaderHandles::Grid)->GetResource();
	}
		
	void GLImmediatePrimitives::Box()
	{

	}

	void GLImmediatePrimitives::Line(const vec3& InStart, const vec3& InEnd, const vec4& InColor, const mat4& InTransformation)
	{
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_LINE_SMOOTH);

		PrimitiveShader->Bind();
		// set pojection
		PrimitiveShader->SetMat4("model", InTransformation);
		PrimitiveShader->SetMat4("view", Camera->GetView());
		PrimitiveShader->SetMat4("projection", Camera->GetProjection());
		PrimitiveShader->SetVec4("myColor", InColor);

		auto vertexArray = GfxGDI::Get()->GetVertexArray(RayVertexArrayId);
		if (vertexArray)
		{
			float vertices[] = {
				InStart.x, InStart.y, InStart.z,
				InEnd.x, InEnd.y, InEnd.z
			};

			auto VertexBuffer = vertexArray->GetVertexBuffer(0);
			VertexBuffer->Reset(vertices, sizeof(vertices));
			

			vertexArray->Bind();
			glDrawArrays(GL_LINE_STRIP, 0, 2);
			vertexArray->Unbind();
		}

		PrimitiveShader->Unbind();
		glDisable(GL_LINE_SMOOTH);
	}

	void GLImmediatePrimitives::Axis(const vec3& InPosition, const vec3& InScale, const quat& InOrientation)
	{
		mat4 transformation = glm::translate(mat4(1), InPosition) * glm::mat4_cast(InOrientation) * glm::scale(mat4(1), InScale);

		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_LINE_SMOOTH);

		PrimitiveShader->Bind();

		PrimitiveShader->SetMat4("model", transformation);
		PrimitiveShader->SetMat4("view", Camera->GetView());
		PrimitiveShader->SetMat4("projection", Camera->GetProjection());
		

		auto vertexArray = GfxGDI::Get()->GetVertexArray(RayVertexArrayId);
		if (vertexArray)
		{
			// X Axis
			{
				PrimitiveShader->SetVec4("color", { 1, 0, 0, 1 });

				float vertices[] = {
					0, 0, 0,
					1, 0, 0,
				};

				auto VertexBuffer = vertexArray->GetVertexBuffer(0);
				VertexBuffer->Reset(vertices, sizeof(vertices));

				vertexArray->Bind();
				glDrawArrays(GL_LINE_STRIP, 0, 2);
				vertexArray->Unbind();
			}

			// Z Axis
			{
				PrimitiveShader->SetVec4("color", { 0, 0, 1, 1 });

				float vertices[] = {
					0, 0, 0,
					0, 0, 1,
				};

				auto VertexBuffer = vertexArray->GetVertexBuffer(0);
				VertexBuffer->Reset(vertices, sizeof(vertices));

				vertexArray->Bind();
				glDrawArrays(GL_LINE_STRIP, 0, 2);
				vertexArray->Unbind();
			}

			// Y Axis
			{
				PrimitiveShader->SetVec4("color", { 0, 1, 0, 1 });

				float vertices[] = {
					0, 0, 0,
					0, 1, 0,
				};

				auto VertexBuffer = vertexArray->GetVertexBuffer(0);
				VertexBuffer->Reset(vertices, sizeof(vertices));

				vertexArray->Bind();
				glDrawArrays(GL_LINE_STRIP, 0, 2);
				vertexArray->Unbind();
			}
		}

		PrimitiveShader->Unbind();
		glDisable(GL_LINE_SMOOTH);
	}
	
	void GLImmediatePrimitives::Grid(const vec3& InPosition, const vec3& InScale)
	{
		mat4 trans(1);
		trans = glm::translate(trans, InPosition) *
			glm::rotate(trans, glm::radians(90.f), vec3(1, 0, 0)) *
			glm::scale(trans, InScale);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		GridShader->Bind();

		if (GridTexture)
		{
			glActiveTexture(GL_TEXTURE0);
			GridShader->SetInt("gridTexture", 0);
			GridTexture->Bind();
		}

		// set pojection
		GridShader->SetMat4("model", trans);
		GridShader->SetMat4("view", Camera->GetView());
		GridShader->SetMat4("projection", Camera->GetProjection());
		GfxGDI::Get()->DrawIndexed(GridVertexArrayId);

		if (GridTexture)
			GridTexture->Unbind();

		GridShader->Unbind();
		glDisable(GL_BLEND);
	}

	void GLImmediatePrimitives::Plane() 
	{

	}

	void GLImmediatePrimitives::DrawRay(const Ray& InRay, float InLength, const vec4& InColor, const mat4& model)
	{

	}

}