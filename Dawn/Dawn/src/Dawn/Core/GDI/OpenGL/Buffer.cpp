#include "stdafx.h"
#include "Buffer.h"
#include "../GfxGDI.h"
#include <glad.h>

namespace Dawn
{
	static GLenum ShaderDataTypeToOpenGLBaseType(GfxShaderDataType type)
	{
		switch (type)
		{
		case GfxShaderDataType::Float:    return GL_FLOAT;
		case GfxShaderDataType::Float2:   return GL_FLOAT;
		case GfxShaderDataType::Float3:   return GL_FLOAT;
		case GfxShaderDataType::Float4:   return GL_FLOAT;
		case GfxShaderDataType::Mat3:     return GL_FLOAT;
		case GfxShaderDataType::Mat4:     return GL_FLOAT;
		case GfxShaderDataType::Int:      return GL_INT;
		case GfxShaderDataType::Int2:     return GL_INT;
		case GfxShaderDataType::Int3:     return GL_INT;
		case GfxShaderDataType::Int4:     return GL_INT;
		case GfxShaderDataType::Bool:     return GL_BOOL;
		}
		return 0;
	}

	GLVertexBuffer::GLVertexBuffer(void* InVertices, u32 InSize, GfxResId InId)
		: GfxVertexBuffer(InId)
	{
		this->Size = InSize;
		glCreateBuffers(1, &RendererId);
		glBindBuffer(GL_ARRAY_BUFFER, RendererId);
		glBufferData(GL_ARRAY_BUFFER, InSize, InVertices, GL_STATIC_DRAW);
	}

	GLVertexBuffer::~GLVertexBuffer()
	{
		glDeleteBuffers(1, &RendererId);
	}

	void GLVertexBuffer::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, RendererId);
	}

	void GLVertexBuffer::Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void GLVertexBuffer::SetLayout(GfxBufferLayout& InLayout)
	{
		Layout = InLayout;
	}

	void GLVertexBuffer::Reset(void* InData, u32 InSize)
	{
		this->Size = InSize;
		Bind();

		glBufferData(GL_ARRAY_BUFFER, InSize, InData, GL_STATIC_DRAW);

		Unbind();
	}

	const GfxBufferLayout& GLVertexBuffer::GetLayout() const
	{
		return Layout;
	}


	GLIndexBuffer::GLIndexBuffer(u32* InData, u32 InSize, GfxResId InId)
		: GfxIndexBuffer(InId)
	{
		this->Size = InSize;
		glCreateBuffers(1, &RendererId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, InSize * sizeof(u32), InData, GL_STATIC_DRAW);
	}

	GLIndexBuffer::~GLIndexBuffer()
	{
		glDeleteBuffers(1, &RendererId);
	}

	void GLIndexBuffer::Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererId);
	}

	void GLIndexBuffer::Unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void GLIndexBuffer::Reset(u32* InData, u32 InSize)
	{
		this->Size = InSize;

		Bind();

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, InSize * sizeof(u32), InData, GL_STATIC_DRAW);

		Unbind();
	}

	GLVertexArray::GLVertexArray(GfxResId InId) 
		: GfxVertexArray(InId)
	{
		glCreateVertexArrays(1, &RendererId);
		//glObjectLabel(GL_VERTEX_ARRAY, RendererId, -1, "Generated Vertex Array from DAWN Engine!");
	}

	GLVertexArray::~GLVertexArray()
	{
		glDeleteVertexArrays(1, &RendererId);
	}


	void GLVertexArray::Bind()
	{
		glBindVertexArray(RendererId);
	}

	void GLVertexArray::Unbind()
	{
		glBindVertexArray(0);
	}

	void GLVertexArray::AttachVertexBuffer(GfxVertexBuffer* InBuffer, i32 InDivisor)
	{
		Bind();

		InBuffer->Bind();

		const auto& layout = InBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(VertexBufferIndex);
			glVertexAttribPointer(VertexBufferIndex,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)(intptr_t)element.Offset);
			

			if (InDivisor > -1)
				glVertexAttribDivisor(VertexBufferIndex, InDivisor);

			VertexBufferIndex++;
		}

		this->Vertices.emplace_back(InBuffer->GetId());
	}

	void GLVertexArray::SetIndexBuffer(GfxIndexBuffer* InBuffer)
	{
		Bind();
		InBuffer->Bind();

		this->Indices = InBuffer->GetId();
	}


	void GLVertexArray::SetName(std::string Name)
	{
		
	}

	GfxVertexBuffer* GLVertexArray::GetVertexBuffer(GfxGDI* InGDI, u32 InIndex)
	{
		auto Id = this->Vertices[InIndex];
		return InGDI->GetVertexBuffer(Id);
	}

	GfxIndexBuffer* GLVertexArray::GetIndexBuffer(GfxGDI* InGDI)
	{
		return InGDI->GetIndexBuffer(this->Indices);
	}
}
