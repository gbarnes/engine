#pragma once
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "Core/Container/Array.h"

namespace Dawn
{
	template<class T> inline T operator~ (T a) { return (T)~(int)a; }
	template<class T> inline T operator| (T a, T b) { return (T)((int)a | (int)b); }
	template<class T> inline T operator& (T a, T b) { return (T)((int)a & (int)b); }
	template<class T> inline T operator^ (T a, T b) { return (T)((int)a ^ (int)b); }
	template<class T> inline T& operator|= (T& a, T b) { return (T&)((int&)a |= (int)b); }
	template<class T> inline T& operator&= (T& a, T b) { return (T&)((int&)a &= (int)b); }
	template<class T> inline T& operator^= (T& a, T b) { return (T&)((int&)a ^= (int)b); }

	typedef GenericHandle GfxResId;

	enum GfxBindFlags : u32 
	{
		Bind_None = 1 << 0,
		Bind_VertexBuffer = 1 << 1, 
		Bind_IndexBuffer = 1 << 2, 
		Bind_ConstantBuffer = 1 << 3,
		Bind_ShaderResource = 1 << 4,
		Bind_StreamOutput = 1 << 5,
		Bind_UnorderedAccess = 1 << 6,
		Bind_RenderTargetView = 1 << 7,
		Bind_DepthStencilView = 1 << 8
	};

	enum GfxCpuAccessFlags : u32
	{
		CpuAccess_None = 1 << 0,
		CpuAccess_Read = 1 << 1,
		CpuAccess_Write = 1 << 2
	};

	enum GfxUsageFlags : u32
	{
		Usage_None = 1 << 0,
		Usage_Default = 1 << 1,
		Usage_Immutable = 1 << 2,
		Usage_Dynamic = 1 << 4,
		Usage_Staging = 1 << 5
	};

	enum class GfxFormat : u8
	{
		R16F,
		RGBA32F,
		RGB32F,
		RGBA16F,
		RGB16F,
		RGBA8UN,
		D24S8 // Depth 24 Bits unsigned normalized integers , Stencil 8 Bits unsigned integers
	};

	enum GfxTextureViewType
	{
		RenderTargetView,
		DepthView,
		DepthStencilView,
		ShaderResourceView
	};

	enum class GfxShaderType : u8
	{
		Pixel,
		Vertex,
		Compute,
		Hull,
		Geometry,
		Num
	};
	
	enum class GfxShaderDataType
	{
		Position, Color, TexCoord0, TexCoord1, Normal, Matrix
	};

	enum class GfxTopologyType
	{
		TopologyUndefined,
		TopologyPoint,
		TopologyLine,
		TopologyPolygon,
		TopologyPatch
	};

	enum GfxMemoryType
	{
		UnsignedByte = 0x1401,
		Float = 0x1406,
		UnsignedInt = 0x1405
	};

	enum class GfxCullMode
	{
		CullNone = 1,
		CullFront = 2,
		CullBack = 3
	};

	enum class GfxFillMode
	{
		FillWireframe = 2,
		FillSolid = 3
	};

	enum class GfxDepthWriteMask {
		DepthWriteMaskZero,
		DepthWriteMaskAll
	};

	enum class GfxComparisonFunc {
		Never,
		Less,
		Equal,
		LessEqual,
		Greater,
		NotEqual,
		GreaterEqual,
		Always
	};

	enum class GfxBlend
	{
		Zero,
		One,
		SrcColor,
		InvSrcColor,
		SrcAlpha,
		InvSrcAlpha,
		DestAlpha,
		InvDestAlpha,
		DestColor,
		InvDestColor,
		SrcAlphaSat,
		BlendFactor,
		InvBlendFactor,
		Src1Color,
		InvSrc1Color,
		Src1Alpha,
		InvSrc1Alpha
	};

	enum class GfxBlendOp 
	{
		OpAdd,
		OpSubtract,
		OpRevSubtract,
		OpMin,
		OpMax
	};

	enum class GfxFilter
	{
		MinMapMipPoint = 0,
		MinMagPointMipLinear = 0x1,
		MinPointMagLinearMiptPoint = 0x4,
		MinPointMagMipLinear = 0x5,
		MinLinearMapMipPoint = 0x10,
		MinLinearMapPointMipLinear = 0x11,
		MinMagLinearMipPoint = 0x14,
		MinMagMipLinear = 0x15,
		Anisotropic = 0x55,
		ComparisonMinMagMipPoint = 0x80,
		ComparisonMinMagPointMipLinear = 0x81,
		ComparisonMinPointMagLinearMipPoint = 0x84,
		ComparisonMinPointMapMipLinear = 0x85,
		ComparisonMinLinearMapMipPoint = 0x90,
		ComparisonMinLinearMagPointMipLinear = 0x91,
		ComparisonMinMagLinearMipPoint = 0x94,
		ComparisonMinMagMipLinear= 0x95,
		ComparisonAnisotropic = 0xd5,
		MinimumMinMagMipPoint = 0x100,
		MinimumMinMagPointMipLinear = 0x101,
		MinimumMinPointMagLinearMipPoint = 0x104,
		MinimumMinPointMagMipLinear = 0x105,
		MinimumMinLinearMagMipPoint = 0x110,
		MinimumMinLinearMagPointMipLinear = 0x111,
		MinimumMinMagLinearMipPoint = 0x114,
		MinimumMinMagMipLinear = 0x115,
		MinimumAnisotropic = 0x155,
		MaximumMinMagMipPoint = 0x180,
		MaximumMinMagPointMipLinear = 0x181,
		MaximumMinPointMagLinearMipPoint = 0x184,
		MaximumMinPointMagMipLinear = 0x185,
		MaximumMinLinearMagMipPoint = 0x190,
		MaximumMinLinearMagPointMipLinear = 0x191,
		MaximumMinMagLinearMipPoint = 0x194,
		MaximumMinMagMipLinear = 0x195,
		MaximumAnisotropic = 0x1d5
	};

	enum class GfxTextureAddressMode
	{
		Wrap = 1,
		Mirror = 2,
		Clamp = 3,
		Border = 4,
		MirrorOnce = 5
	};

	static uint32_t ShaderDataTypeSize(GfxShaderDataType type)
	{
		switch (type)
		{
		case GfxShaderDataType::Position:   return 4 * 3;
		case GfxShaderDataType::Normal:  return 4 * 3;
		case GfxShaderDataType::TexCoord0:  return 4 * 2;
		case GfxShaderDataType::TexCoord1:  return 4 * 2;
		case GfxShaderDataType::Color:  return 4 * 4;
		case GfxShaderDataType::Matrix:    return 4 * 4 * 4;
		}
		return 0;
	}

	struct GfxInputElement
	{
		std::string Name;
		GfxShaderDataType Type;
		std::string SemanticName;
		uint32_t Size;
		uint32_t Offset;
		bool Normalized;
		bool IsPerInstance = false;

		GfxInputElement() {}

		GfxInputElement(GfxShaderDataType type, const std::string& name, bool normalized = false, bool isPerInstance = false)
			: Name(name), SemanticName(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized), IsPerInstance(isPerInstance)
		{
		}

		GfxInputElement(GfxShaderDataType type, const std::string& name, const std::string& semanticName, bool normalized = false, bool isPerInstance = false)
			: Name(name), SemanticName(semanticName), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized), IsPerInstance(isPerInstance)
		{
		}


		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
			case GfxShaderDataType::Position:   return 3;
			case GfxShaderDataType::Normal:  return 3;
			case GfxShaderDataType::TexCoord0:  return 2;
			case GfxShaderDataType::TexCoord1:  return 2;
			case GfxShaderDataType::Color:  return 4;
			case GfxShaderDataType::Matrix:    return 4 * 4;
			/*case GfxShaderDataType::Mat3:    return 3 * 3;
			case GfxShaderDataType::Mat4:    return 4 * 4;
			case GfxShaderDataType::Int:     return 1;
			case GfxShaderDataType::Int2:    return 2;
			case GfxShaderDataType::Int3:    return 3;
			case GfxShaderDataType::Int4:    return 4;
			case GfxShaderDataType::Bool:    return 1;*/
			}
			return 0;
		}
	};

	class GfxInputLayout
	{
	public:
		GfxInputLayout() {}

		GfxInputLayout(const std::initializer_list<GfxInputElement>& elements)
			: Elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		inline uint32_t GetStride() const { return Stride; }
		inline const std::vector<GfxInputElement>& GetElements() const { return Elements; }
		inline i64 Count() { return Elements.size(); }
		std::vector<GfxInputElement>::iterator Begin() { return Elements.begin(); }
		std::vector<GfxInputElement>::iterator End() { return Elements.end(); }
		std::vector<GfxInputElement>::const_iterator Begin() const { return Elements.begin(); }
		std::vector<GfxInputElement>::const_iterator End() const { return Elements.end(); }
	private:
		void CalculateOffsetsAndStride()
		{
			uint32_t offset = 0;
			Stride = 0;
			for (auto& element : Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				Stride += element.Size;
			}
		}
	private:
		std::vector<GfxInputElement> Elements;
		uint32_t Stride = 0;
	};

	struct GfxState
	{
		bool bEnableDepthTest = true;
		bool bEnableCulling = true;
		GfxCullMode FaceCull = GfxCullMode::CullBack;
	};

/*	enum class GfxResourceMiscFlags : u8 {
		MiscGenerateMips = 0x01,
		MiscShared = 0x02,
		MiscTextureCube = 0x04,
		MiscDrawIndirectARgs = 0x05,

		D3D11_RESOURCE_MISC_GENERATE_MIPS,
		D3D11_RESOURCE_MISC_SHARED,
		D3D11_RESOURCE_MISC_TEXTURECUBE,
		D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS,
		D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
		D3D11_RESOURCE_MISC_RESOURCE_CLAMP,
		D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX,
		D3D11_RESOURCE_MISC_GDI_COMPATIBLE,
		D3D11_RESOURCE_MISC_SHARED_NTHANDLE,
		D3D11_RESOURCE_MISC_RESTRICTED_CONTENT,
		D3D11_RESOURCE_MISC_RESTRICT_SHARED_RESOURCE,
		D3D11_RESOURCE_MISC_RESTRICT_SHARED_RESOURCE_DRIVER,
		D3D11_RESOURCE_MISC_GUARDED,
		D3D11_RESOURCE_MISC_TILE_POOL,
		D3D11_RESOURCE_MISC_TILED,
		D3D11_RESOURCE_MISC_HW_PROTECTED
	};*/

	struct GfxWrapDesc
	{
		i32 WrapS;
		i32 WrapT;
	};

	struct GfxFilterDesc
	{
		i32 Min;
		i32 Mag;
	};

	struct GfxViewport
	{
		i32 TopLeftX;
		i32 TopLeftY;
		u32 Width;
		u32 Height;
	};

	struct GfxShaderDesc
	{
		GfxShaderType Type;
		const char* FileName;
		const char* FunctionName;
		const char* ProfileName;
	};

	struct GfxShaderData
	{
		const char* Data;
		u32 Size;
	};

	struct GfxBufferDesc
	{
		u32 ByteWidth;
		GfxUsageFlags Usage;
		GfxCpuAccessFlags AccessFlags;
		GfxBindFlags BindFlags;
		//GfxResourceMiscFlags MiscFlags;
		u32 StructureByteStride;
	};

	struct GfxBufferData
	{
		const void* Data;
		u32 Size;
	};

	struct GfxTextureData
	{
		const void* Data;
		u32 Size;
		u16 ChannelsPerPixel = 8;
	};

	class GfxTexture;
	struct GfxTextureViewDesc
	{
		GfxTextureViewType Type;
		void* AdressToGPU;
	};

	struct GfxTextureDesc
	{
		GfxFormat Format = GfxFormat::RGBA16F;
		u32 Width = 10;
		u32 Height = 10;
		GfxWrapDesc Wrap;
		GfxFilterDesc Filter;
		GfxCpuAccessFlags CpuAccess;
		GfxUsageFlags Usage;
		GfxBindFlags BindFlags;
		u32 MipLevels = 0;
		u32 ArraySize = 0;
		u32 SampleCount = 1;
		u32 SampleQuality = 1;
	};

	struct GfxRenderTargetBlendDesc 
	{
		i32 BlendEnable;
		GfxBlend SrcBlend;
		GfxBlend DestBlend;
		GfxBlendOp BlendOp;
		GfxBlend SrcBlendAlpha;
		GfxBlend DestBlendAlpha;
		GfxBlendOp BlendOpAlpha;
		u8 RenderTargetWriteMask;
	};

	struct GfxBlendDesc
	{
		i32 AlphaToCoverageEnable;
		i32 IndependentBlendEnable;
		GfxRenderTargetBlendDesc RenderTarget[8];
	};

	struct GfxRasterizerDesc
	{
		GfxFillMode FillMode;
		GfxCullMode CullMode;
		i32 FrontCounterClockwise;
		i32 DepthBias;
		float DepthBiasClamp;
		float SlopeScaledDepthBias;
		i32 DepthClipEnable;
		i32 MultisampleEnable;
		i32 AntialiasedLineEnable;
		u32 ForcedSampleCount;
	};

	struct GfxDepthStencilDesc
	{
		i32 DepthEnable;
		GfxDepthWriteMask DepthWriteMask;
		GfxComparisonFunc DepthFunc;
		i32 StencilEnable;
		u8 StencilReadMask;
		u8 StencilWriteMask;
		//D3D12_DEPTH_STENCILOP_DESC FrontFace;
		//D3D12_DEPTH_STENCILOP_DESC BackFace;
		
	};

	struct GfxPipelineStateObjectDesc
	{
		GfxTopologyType TopologyType;
		GfxInputLayout InputLayout;
		GfxResId VertexShaderId;
		GfxResId PixelShaderId;
		GfxResId ComputeShaderId;
		GfxResId GeometryShaderId;
		GfxResId HullShaderId;
		GfxBlendDesc BlendState;
		GfxRasterizerDesc RasterizerState;
		GfxDepthStencilDesc DepthStencilState;
	};

	struct GfxSamplerDesc
	{
		GfxFilter Filter;
		GfxTextureAddressMode AddressU;
		GfxTextureAddressMode AddressV;
		GfxTextureAddressMode AddressW;
		float MipLODBias;
		u32 MaxAnisotropy;
		GfxComparisonFunc ComparisonFunc;
		float BorderColor[4];
		float MinLOD;
		float MaxLOD;
	};

	template<typename T>
	class ScopedGfxBind
	{
	public:
		ScopedGfxBind()
		{
		}

		ScopedGfxBind(T* InResource)
		{
			Bind(InResource);
		}

		~ScopedGfxBind()
		{
			if (Resource)
				Resource->Unbind();
		}

		void Bind(T* InResource)
		{
			if (InResource)
			{
				Resource = InResource;
				Resource->Bind();
			}
		}

	private:
		T* Resource = nullptr;
	};
};