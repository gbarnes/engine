#include "Buffer.h"
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

	GLVertexBuffer::~GLVertexBuffer()
	{
		glDeleteBuffers(1, &RendererId);
	}

	void GLIndexBuffer::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, RendererId);
	}

	void GLIndexBuffer::Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void GLVertexBuffer::Reset(const GfxBufferLayout& InLayout, float* InData, u32 InSize)
	{
		Layout = InLayout;

		glDeleteBuffers(1, &RendererId);
		glGenBuffers(1, &RendererId);

		Bind();

		glBufferData(GL_ARRAY_BUFFER, sizeof(InData) * InSize, &InData[0], GL_STATIC_DRAW);

		Unbind();
	}

	const GfxBufferLayout& GLVertexBuffer::GetLayout() const
	{
		return Layout;
	}


	GLIndexBuffer::GLIndexBuffer(GfxResId InId)
		: GfxIndexBuffer(InId)
	{
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

	void GLIndexBuffer::Reset(u16* InData, u32 InSize)
	{
		glDeleteBuffers(1, &RendererId);
		glGenBuffers(1, &RendererId);
		
		Bind();

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, InSize * sizeof(u16), &InData[0], GL_STATIC_DRAW);

		Unbind();
	}

	GLVertexArray::GLVertexArray(GfxResId InId) 
		: GfxVertexArray(InId)
	{
		glGenVertexArrays(1, &RendererId);
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

	void GLVertexArray::AttachVertexBuffer(const std::shared_ptr<GfxVertexBuffer>& InBuffer)
	{
		Bind();

		InBuffer->Bind();

		uint32_t index = 0;
		const auto& layout = InBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset);
			index++;
		}

		this->Vertices.emplace_back(InBuffer);

		InBuffer->Unbind();
	}

	void GLVertexArray::SetIndexBuffer(const std::shared_ptr<GfxIndexBuffer>& InBuffer)
	{

		Bind();
		InBuffer->Bind();

		this->Indices = InBuffer;

		InBuffer->Unbind();
		Unbind();
	}
}
