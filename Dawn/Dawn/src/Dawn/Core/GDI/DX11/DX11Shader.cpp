#include "stdafx.h"
#include "DX11Shader.h"
#include "Core/Logging/Log.h"
#include "DX11GDI.h"
#include "inc_dx11.h"

namespace Dawn
{
	DX11Shader::DX11Shader(const GfxResId& InId, GfxGDIPtr InGDI)
		: GfxShader(InId, InGDI), Shader(nullptr), ByteCode(nullptr)
	{
		
	}

	DX11Shader::~DX11Shader()
	{
		ByteCode = nullptr;
		Shader = nullptr;
	}

	void DX11Shader::SetName(const std::string& InName)
	{
		Shader->SetPrivateData(WKPDID_D3DDebugObjectName, InName.size(), InName.c_str());
	}

	void DX11Shader::Create(const GfxShaderDesc& InDesc, const GfxShaderData& InData)
	{
		Shader = nullptr;
		ByteCode = nullptr;

		this->Type = InDesc.Type;
		DX11GDI* gdi = static_cast<DX11GDI*>(this->GDI.get());

#ifdef _DEBUG
		// todo (gb): we should pre-compile the shader in release builds but for now its hot compiled of course!
		ComPtr<ID3D10Blob> errorBlob;
		HRESULT result = D3DX11CompileFromMemory(InData.Data, InData.Size, InDesc.FileName, nullptr, nullptr, InDesc.FunctionName, InDesc.ProfileName,
			0, 0, nullptr, &ByteCode, &errorBlob, nullptr);

		if (FAILED(result))
		{
			if (errorBlob != 0)
			{
				DWN_ERROR((char*)errorBlob->GetBufferPointer());
			}

			return;
		}
#endif

		if (this->Type == Dawn::GfxShaderType::Vertex)
		{
			ComPtr<ID3D11VertexShader> vs;
			HRESULT result = gdi->GetD3D11Device()->CreateVertexShader(ByteCode->GetBufferPointer(), ByteCode->GetBufferSize(), nullptr, &vs);
			D_ASSERT(SUCCEEDED(result), "Couldn't create vertex shader");
			Shader = vs;
		}
		else if (this->Type == Dawn::GfxShaderType::Pixel)
		{
			ComPtr<ID3D11PixelShader> ps;
			HRESULT result = gdi->GetD3D11Device()->CreatePixelShader(ByteCode->GetBufferPointer(), ByteCode->GetBufferSize(), nullptr, &ps);
			D_ASSERT(SUCCEEDED(result), "Couldn't create pixel shader");
			Shader = ps;
		}
		else if (this->Type == Dawn::GfxShaderType::Compute)
		{
			ComPtr<ID3D11ComputeShader> cs;
			HRESULT result = gdi->GetD3D11Device()->CreateComputeShader(ByteCode->GetBufferPointer(), ByteCode->GetBufferSize(), nullptr, &cs);
			D_ASSERT(SUCCEEDED(result), "Couldn't create compute shader");
			Shader = cs;
		}
		else if (this->Type == Dawn::GfxShaderType::Geometry)
		{
			ComPtr<ID3D11GeometryShader> gs;
			HRESULT result = gdi->GetD3D11Device()->CreateGeometryShader(ByteCode->GetBufferPointer(), ByteCode->GetBufferSize(), nullptr, &gs);
			D_ASSERT(SUCCEEDED(result), "Couldn't create geometry shader");
			Shader = gs;
		}
		else if (this->Type == Dawn::GfxShaderType::Hull)
		{
			ComPtr<ID3D11HullShader> hs;
			HRESULT result = gdi->GetD3D11Device()->CreateHullShader(ByteCode->GetBufferPointer(), ByteCode->GetBufferSize(), nullptr, &hs);
			D_ASSERT(SUCCEEDED(result), "Couldn't create hull shader");
			Shader = hs;
		}

		SetName(fmt::format("Shader {0}", InDesc.FileName));

		return;
	}
}