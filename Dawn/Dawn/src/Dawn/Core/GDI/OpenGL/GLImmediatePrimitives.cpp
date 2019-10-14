#include "GLImmediatePrimitives.h"
#include "../inc_gfx.h"
#include "glad.h"
#include "EntitySystem/Camera/Camera.h"
#include "ResourceSystem/Resources.h"
#include "Rendering/RenderResourceHelper.h"
#include "../GfxGDI.h"

namespace Dawn
{
	GfxResId GridVertexArrayId, RayVertexArrayId, QuadVertexArrayId;
	GfxTexture* GridTexture;
	GfxShader* PrimitiveShader;
	GfxShader* GridShader;

	GLImmediatePrimitives::GLImmediatePrimitives(Shared<GfxGDI> InGDI)
		: GfxImmediatePrimitives(InGDI)
	{
	}

	void GLImmediatePrimitives::AllocateBuffers(Image* InGridImage, Shader* InPrimitiveShader, Shader* InGridShader)
	{
		// Textured-Grid Allocation
		{
			GridVertexArrayId = GfxPrimitiveFactory::AllocateQuad(GDI.get(), vec2(1000.0f, 1000.0f))->GetId();

			if (InGridImage)
			{
				GridTexture = GDI->GetTexture(InGridImage->TextureId);
			}

			QuadVertexArrayId = GfxPrimitiveFactory::AllocateQuad(GDI.get(), vec2(1, 1))->GetId();
		}

		RayVertexArrayId = GfxPrimitiveFactory::AllocateLine(GDI.get())->GetId();

		// Note (gb): this might be better off passing as an argument, too!
		PrimitiveShader = GDI->GetShader(InPrimitiveShader->ResourceId);
		GridShader = GDI->GetShader(InGridShader->ResourceId);
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

		auto vertexArray = GDI->GetVertexArray(RayVertexArrayId);
		if (vertexArray)
		{
			float vertices[] = {
				InStart.x, InStart.y, InStart.z,
				InEnd.x, InEnd.y, InEnd.z
			};

			auto VertexBuffer = vertexArray->GetVertexBuffer(GDI.get(), 0);
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
		mat4 model = mat4(1);
		model = glm::translate(model, InPosition) * glm::mat4_cast(InOrientation) * glm::scale(model, InScale);
		

		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_LINE_SMOOTH);

		PrimitiveShader->Bind();

		PrimitiveShader->SetMat4("model", model);
		PrimitiveShader->SetMat4("view", Camera->GetView());
		PrimitiveShader->SetMat4("projection", Camera->GetProjection());
		

		auto vertexArray = GDI->GetVertexArray(RayVertexArrayId);
		if (vertexArray)
		{
			// X Axis
			{
				PrimitiveShader->SetVec4("color", { 1, 0, 0, 1 });

				float vertices[] = {
					0, 0, 0,
					1, 0, 0,
				};

				auto VertexBuffer = vertexArray->GetVertexBuffer(GDI.get(), 0);
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
					0, 0, -1,
				};

				auto VertexBuffer = vertexArray->GetVertexBuffer(GDI.get(), 0);
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

				auto VertexBuffer = vertexArray->GetVertexBuffer(GDI.get(), 0);
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
		GDI->DrawIndexed(GridVertexArrayId);

		if (GridTexture)
			GridTexture->Unbind();

		GridShader->Unbind();
		glDisable(GL_BLEND);
	}

	void GLImmediatePrimitives::Quad(GfxTexture* InTexture, const vec3& InPosition, const vec3& InScale, const quat& InOrientation)
	{
		mat4 trans(1);
		trans = glm::translate(trans, InPosition) *
			glm::mat4_cast(InOrientation) *
			glm::scale(trans, InScale);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		GridShader->Bind();

		if (InTexture)
		{
			GDI->ActivateTextureSlot(0);
			GridShader->SetInt("gridTexture", 0);
			InTexture->Bind();
		}

		// set pojection
		GridShader->SetMat4("model", trans);
		GridShader->SetMat4("view", Camera->GetView());
		GridShader->SetMat4("projection", Camera->GetProjection());
		GDI->DrawIndexed(QuadVertexArrayId);

		if (InTexture)
			InTexture->Unbind();

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