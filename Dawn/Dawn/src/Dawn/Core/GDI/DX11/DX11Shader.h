#pragma once
#include "Core/GDI/Base/GfxShader.h"
#include "Core/GDI/Base/GfxGDI.h"
#include "Core/GDI/Base/inc_gfx_types.h"

struct ID3D11DeviceChild;
struct ID3D10Blob;

namespace Dawn
{
	class DX11Shader : public GfxShader
	{
	public:
		DX11Shader(const GfxResId& InId, GfxGDIPtr InGDI);
		virtual ~DX11Shader();
		
		virtual void Create(const GfxShaderDesc& InDesc, const GfxShaderData& InData) override;
		virtual void SetName(const std::string& InName) override;

		inline ID3D11DeviceChild* GetD3D11Shader() const { return Shader.Get(); }
		inline ID3D10Blob* GetD3D11Blob() const { return ByteCode.Get(); }
		inline GfxShaderType GetType() const { return Type; }

	private:
		GfxShaderType Type;
		ComPtr<ID3D11DeviceChild> Shader;
		ComPtr<ID3D10Blob> ByteCode;
	};

}