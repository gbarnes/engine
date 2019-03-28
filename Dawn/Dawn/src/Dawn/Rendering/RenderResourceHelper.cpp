#include "RenderResourceHelper.h"
#include "Core/GDI/inc_gfx_types.h"
#include "Core/GDI/GfxBackend.h"
#include "ResourceSystem/ResourceSystem.h"
#include "ResourceSystem/ResourceTable.h"
#include "Core/GDI/GfxRootSignature.h"

namespace Dawn 
{
	void RenderResourceHelper::LoadCommonShaders()
	{
		// Create the common shader resources in the order 
		// the enum is structured!
		RefPtr<ResourceSystem> rs(ResourceSystem::Get());
		CommonShaders.ID_PhongVS = rs->LoadFile("Shader/phong_vs.cso");
		CommonShaders.ID_PhongPS = rs->LoadFile("Shader/phong_ps.cso");

		CommonShaders.ID_DebugVS = rs->LoadFile("Shader/debug_vs.cso");
		CommonShaders.ID_DebugPS = rs->LoadFile("Shader/debug_ps.cso");

		// Root Signature for phong shader
		{
			CD3DX12_DESCRIPTOR_RANGE1 textureSRV(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
			CD3DX12_DESCRIPTOR_RANGE1 textureSampler(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

			CD3DX12_ROOT_PARAMETER1 rootParameters[2];
			rootParameters[0].InitAsConstants(sizeof(DirectX::XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
			rootParameters[1].InitAsDescriptorTable(1, &textureSRV, D3D12_SHADER_VISIBILITY_PIXEL);

			GfxBackend::CreateRootSignature(2, rootParameters, 1, &g_AnisotropicWrapSampler, g_RootSignaturePSVSFlags);
		}

		// Root Signature for debug rendering shader
		{
			CD3DX12_ROOT_PARAMETER1 rootParameters[1];
			rootParameters[0].InitAsConstants(sizeof(DirectX::XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
			GfxBackend::CreateRootSignature(1, rootParameters, 0, nullptr, g_RootSignaturePSVSFlags);
		}

	}

	void RenderResourceHelper::CreateCommonPipelineStates()
	{
		RefPtr<ResourceSystem> rs(ResourceSystem::Get());

		// Create pipeline state for phong shaders
		{
			if (CommonShaders.ID_PhongPS.IsValid && CommonShaders.ID_PhongVS.IsValid)
			{
				auto psShader = ResourceTable::GetShader(CommonShaders.ID_PhongPS);
				auto vsShader = ResourceTable::GetShader(CommonShaders.ID_PhongVS);
				auto rs = GfxBackend::GetRootSignature(RS_Standard).get();

				D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
				desc.pRootSignature = rs->GetRootSignature().Get();
				desc.InputLayout = { PosUVInputLayout, _countof(PosUVInputLayout) };
				desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
				desc.VS = CD3DX12_SHADER_BYTECODE(vsShader->D3DData.Get());
				desc.PS = CD3DX12_SHADER_BYTECODE(psShader->D3DData.Get());
				desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
				desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.NumRenderTargets = 1;
				desc.NodeMask = 0;
				desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
				desc.RasterizerState.FillMode = D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID;
				desc.RasterizerState.CullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK;
				desc.RasterizerState.FrontCounterClockwise = false;
				desc.DepthStencilState.DepthEnable = true;
				desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS;
				desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK::D3D12_DEPTH_WRITE_MASK_ALL;
				desc.DepthStencilState.StencilEnable = false;
				desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

				desc.SampleDesc.Count = 1;
				desc.SampleDesc.Quality = 0;
				desc.SampleMask = UINT_MAX;

				GfxBackend::CreatePipelineState(&desc);
			}
		}

		// Create pipelinestates for debug elements
		{
			if (CommonShaders.ID_DebugPS.IsValid && CommonShaders.ID_DebugVS.IsValid)
			{
				auto psShader = ResourceTable::GetShader(CommonShaders.ID_DebugPS);
				auto vsShader = ResourceTable::GetShader(CommonShaders.ID_DebugVS);
				auto rs = GfxBackend::GetRootSignature(RS_Debug).get();

				D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
				desc.pRootSignature = rs->GetRootSignature().Get();
				desc.InputLayout = { PosColorInputLayout, _countof(PosColorInputLayout) };
				desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
				desc.VS = CD3DX12_SHADER_BYTECODE(vsShader->D3DData.Get());
				desc.PS = CD3DX12_SHADER_BYTECODE(psShader->D3DData.Get());
				desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
				desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.NumRenderTargets = 1;
				desc.NodeMask = 0;
				desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
				desc.RasterizerState.FillMode = D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID;
				desc.RasterizerState.CullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK;
				desc.RasterizerState.FrontCounterClockwise = false;
				desc.DepthStencilState.DepthEnable = false;
				desc.DepthStencilState.StencilEnable = false;
				desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

				desc.SampleDesc.Count = 1;
				desc.SampleDesc.Quality = 0;
				desc.SampleMask = UINT_MAX;

				GfxBackend::CreatePipelineState(&desc);

				desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
				GfxBackend::CreatePipelineState(&desc);
			}
		}
	}
}