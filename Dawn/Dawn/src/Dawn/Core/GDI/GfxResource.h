#pragma once
#include "inc_common.h"
#include "inc_gfx_types.h"

namespace Dawn
{
	class GfxGDI;

	class GfxResource
	{
	public:
		GfxResource(GfxResId InId)
			: Id(InId)
		{
		}

		GfxResId GetId() const
		{
			return Id;
		}

	protected:
		std::string Name;
		GfxResId Id;

	};

	// 
	// The Buffer Layout class and associated classes have been taken 
	// from the Hazel Engine Tutorial Series!
	//

	enum class GfxShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static uint32_t ShaderDataTypeSize(GfxShaderDataType type)
	{
		switch (type)
		{
		case GfxShaderDataType::Float:    return 4;
		case GfxShaderDataType::Float2:   return 4 * 2;
		case GfxShaderDataType::Float3:   return 4 * 3;
		case GfxShaderDataType::Float4:   return 4 * 4;
		case GfxShaderDataType::Mat3:     return 4 * 3 * 3;
		case GfxShaderDataType::Mat4:     return 4 * 4 * 4;
		case GfxShaderDataType::Int:      return 4;
		case GfxShaderDataType::Int2:     return 4 * 2;
		case GfxShaderDataType::Int3:     return 4 * 3;
		case GfxShaderDataType::Int4:     return 4 * 4;
		case GfxShaderDataType::Bool:     return 1;
		}
		return 0;
	}

	struct GfxBufferElement
	{
		std::string Name;
		GfxShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;
		bool Normalized;

		GfxBufferElement() {}

		GfxBufferElement(GfxShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
		{
		}

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
			case GfxShaderDataType::Float:   return 1;
			case GfxShaderDataType::Float2:  return 2;
			case GfxShaderDataType::Float3:  return 3;
			case GfxShaderDataType::Float4:  return 4;
			case GfxShaderDataType::Mat3:    return 3 * 3;
			case GfxShaderDataType::Mat4:    return 4 * 4;
			case GfxShaderDataType::Int:     return 1;
			case GfxShaderDataType::Int2:    return 2;
			case GfxShaderDataType::Int3:    return 3;
			case GfxShaderDataType::Int4:    return 4;
			case GfxShaderDataType::Bool:    return 1;
			}
			return 0;
		}
	};

	class GfxBufferLayout
	{
	public:
		GfxBufferLayout() {}

		GfxBufferLayout(const std::initializer_list<GfxBufferElement>& elements)
			: m_Elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		inline uint32_t GetStride() const { return m_Stride; }
		inline const std::vector<GfxBufferElement>& GetElements() const { return m_Elements; }

		std::vector<GfxBufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<GfxBufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<GfxBufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<GfxBufferElement>::const_iterator end() const { return m_Elements.end(); }
	private:
		void CalculateOffsetsAndStride()
		{
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}
	private:
		std::vector<GfxBufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	class GfxVertexBuffer : public GfxResource
	{
	public:
		GfxVertexBuffer(GfxResId InId)
			: GfxResource(InId)
		{
		}

		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void Reset(float* InData, u32 InSize) = 0;
		virtual void SetLayout(GfxBufferLayout& Layout) = 0;
		virtual const GfxBufferLayout& GetLayout() const = 0;
	};

	class GfxIndexBuffer : public GfxResource
	{
	public:
		GfxIndexBuffer(GfxResId InId)
			: GfxResource(InId)
		{
		}

		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void Reset(u32* InData, u32 InSize) = 0;

		inline u32 GetSize() const
		{
			return Size;
		}

	protected:
		u32 Size;
	};

	class GfxVertexArray : public GfxResource
	{
	public:
		GfxVertexArray(GfxResId InId)
			: GfxResource(InId)
		{
		}

		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void AttachVertexBuffer(GfxVertexBuffer* InBuffer) = 0;
		virtual void SetIndexBuffer(GfxIndexBuffer* InBuffer) = 0;
		virtual GfxVertexBuffer* GetVertexBuffer(GfxGDI* GDI, u32 InIndex) = 0;
		virtual GfxIndexBuffer* GetIndexBuffer(GfxGDI* GDI) = 0;
		virtual void SetName(std::string Name) = 0;
	};

	class GfxTexture : public GfxResource
	{
	public:
		GfxTexture(GfxResId InId) : GfxResource(InId) {};
		virtual ~GfxTexture() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		inline const GfxResId GetId() const {
			return Id;
		}
	};

	enum GfxShaderType
	{
		ST_Pixel,
		ST_Vertex,
		ST_Compute
	};

	class GfxShader : public GfxResource
	{
	public:
		GfxShader(GfxResId InId) : GfxResource(InId) {};
		virtual ~GfxShader() = default;

		virtual bool AttachSource(GfxShaderType InType, const char* InShaderCode) = 0;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		inline const GfxResId GetId() const {
			return Id;
		}

	public:
		virtual void SetInt(const std::string& Name, i32 Value) = 0;
		virtual void SetFloat(const std::string& Name, float Value) = 0;
		virtual void SetVec2(const std::string& Name, const vec2& Value) = 0;
		virtual void SetVec3(const std::string& Name, const vec3& Value) = 0;
		virtual void SetVec4(const std::string& Name, const vec4& Value) = 0;
		virtual void SetMat4(const std::string& Name, const mat4& Value) = 0;
	};


	typedef std::shared_ptr<GfxTexture> GfxTexturePtr;
	typedef std::shared_ptr<GfxShader> GfxShaderPtr;
	typedef std::shared_ptr<GfxVertexBuffer> GfxVertexBufferPtr;
	typedef std::shared_ptr<GfxIndexBuffer> GfxIndexBufferPtr;
	typedef std::shared_ptr<GfxVertexArray> GfxVertexArrayPtr;
}