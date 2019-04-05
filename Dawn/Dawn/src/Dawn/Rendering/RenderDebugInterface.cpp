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
	VertexPosUV1 vertices[] = {
		{vec3(-0.5f,  0.5f, 0.0f), vec2(0.0f, 0.0f) },
		{vec3(0.5f,  0.5f,  0.0f), vec2(1000.0f, 0.0f) },
		{vec3(0.5f, -0.5f,  0.0f), vec2(1000.0f, 1000.0f) },
		{vec3(-0.5f, -0.5f, 0.0f), vec2(0.0f, 1000.0f) },
	};

	u32 indices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	u32 VBO, VAO, EBO, RAYVBO, RAYVAO;
	std::shared_ptr<Image> GridImage;

	

	void Debug::AllocateResources()
	{
		// Grid Vertex Objects
		{
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			// vertex positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosUV1), (void*)0);
			// vertex texture coords0
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPosUV1), (void*)offsetof(VertexPosUV1, UV0));

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
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
			CopyImagesToGPU({ &imagePtr }, 1, { GL_REPEAT, GL_REPEAT }, { GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR }, true);
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

		mat4 trans(1);
		trans = glm::translate(trans, InPosition) * glm::rotate(trans, glm::radians(90.f), vec3(1, 0, 0)) * glm::scale(trans, vec3(1000, 1000, 1000));
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUseProgram(shader->GDI_ShaderId);

		glActiveTexture(GL_TEXTURE0);

		glUniform1i(glGetUniformLocation(shader->GDI_ShaderId, "gridTexture"), 0);
		glBindTexture(GL_TEXTURE_2D, GridImage->GDI_TextureId);

		// set pojection
		glUniformMatrix4fv(glGetUniformLocation(shader->GDI_ShaderId, "model"), 1, GL_FALSE, &trans[0][0]);

		// set pojection
		glUniformMatrix4fv(glGetUniformLocation(shader->GDI_ShaderId, "view"), 1, GL_FALSE, &InCamera->GetView()[0][0]);

		// set pojection
		glUniformMatrix4fv(glGetUniformLocation(shader->GDI_ShaderId, "projection"), 1, GL_FALSE, &InCamera->GetProjection()[0][0]);
		
		
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glUseProgram(0);
		glBindVertexArray(0);
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
		mat4 trans(1);

		VertexPos ray[] = {
			{InRay.Pos},
			{InRay.Pos + (InRay.Dir) * InLength}
		};

		u32 vbo = 0;
		
		glUseProgram(shader->GDI_ShaderId);
		// set pojection
		glUniformMatrix4fv(glGetUniformLocation(shader->GDI_ShaderId, "model"), 1, GL_FALSE, &trans[0][0]);

		// set pojection
		glUniformMatrix4fv(glGetUniformLocation(shader->GDI_ShaderId, "view"), 1, GL_FALSE, &InCamera->GetView()[0][0]);

		// set pojection
		glUniformMatrix4fv(glGetUniformLocation(shader->GDI_ShaderId, "projection"), 1, GL_FALSE, &InCamera->GetProjection()[0][0]);

		glUniform4fv(glGetUniformLocation(shader->GDI_ShaderId, "myColor"), 1, &InColor[0]);

		glBindVertexArray(RAYVAO);
		glBindBuffer(GL_ARRAY_BUFFER, RAYVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ray), ray, GL_STATIC_DRAW);

		glDrawArrays(GL_LINE_STRIP, 0, 2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glUseProgram(0);
	}


}