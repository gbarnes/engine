#include "stdafx.h"
#include "RenderResourceHelper.h"
#include "Core/GDI/Base/inc_gfx_types.h"
#include "Core/Container/Map.h"
#include "ResourceSystem/ResourceSystem.h"
#include "Application.h"
#include "Core/GDI/Base/GfxInputLayouts.h"

namespace Dawn 
{
	ResourceId CommonShaderHandles::FXAA;
	ResourceId CommonShaderHandles::LightingPass;
	ResourceId CommonShaderHandles::DebugPrim;
	ResourceId CommonShaderHandles::Phong;
	ResourceId CommonShaderHandles::Debug;
	ResourceId EditorShaderHandles::Grid;
	ResourceId CommonShaderHandles::Standard;
	ResourceId CommonShaderHandles::StandardInstanced;
	ResourceId CommonShaderHandles::SSAOCompute;
	ResourceId CommonShaderHandles::SSAOBlur;
	ResourceId CommonShaderHandles::ShadowMapCompute;

	ResourceId CommonShaderHandles::DebugVS;
	ResourceId CommonShaderHandles::DebugPS;
	ResourceId CommonShaderHandles::DebugInstancedVS;
	ResourceId CommonShaderHandles::ShadowMapVS;
	ResourceId CommonShaderHandles::ColoredSimplePS;

	void RenderResourceHelper::LoadCommonShaders(ResourceSystem* InResourceSystem)
	{
		/*CommonShaderHandles::FXAA = InResourceSystem->LoadFile("Shader/fxaa.shader");
		CommonShaderHandles::LightingPass = InResourceSystem->LoadFile("Shader/lighting_pass.shader");
		CommonShaderHandles::Standard = InResourceSystem->LoadFile("Shader/standard.shader");
		CommonShaderHandles::StandardInstanced = InResourceSystem->LoadFile("Shader/standard_instanced.shader");
		CommonShaderHandles::DebugPrim = InResourceSystem->LoadFile("Shader/debug_prim.shader");
		CommonShaderHandles::Debug = InResourceSystem->LoadFile("Shader/debug_gl.shader");
		CommonShaderHandles::SSAOCompute = InResourceSystem->LoadFile("Shader/ssao_compute.shader");
		CommonShaderHandles::SSAOBlur = InResourceSystem->LoadFile("Shader/ssao_blur.shader");
		EditorShaderHandles::Grid = InResourceSystem->LoadFile("Shader/editor_grid.shader");
		CommonShaderHandles::ShadowMapCompute = InResourceSystem->LoadFile("Shader/shadowmap_compute.shader");*/

		CommonShaderHandles::DebugInstancedVS = InResourceSystem->LoadFile("Shader/HLSL/default_instanced.h_vs");
		CommonShaderHandles::DebugVS = InResourceSystem->LoadFile("Shader/HLSL/default.h_vs");
		CommonShaderHandles::DebugPS = InResourceSystem->LoadFile("Shader/HLSL/default.h_ps");

		CommonShaderHandles::ShadowMapVS = InResourceSystem->LoadFile("Shader/HLSL/shadowpass.h_vs");
		CommonShaderHandles::ColoredSimplePS = InResourceSystem->LoadFile("Shader/HLSL/colored_simple.h_ps");
	}

	GfxResId CommonConstantBuffers::PerAppData;
	GfxResId CommonConstantBuffers::PerObjectData;
	GfxResId CommonConstantBuffers::PerFrameData;

	Map<std::string, Dawn::GfxResId> CachedObjects;

	void RenderResourceHelper::CreateConstantBuffers(GfxGDI* InGDI)
	{
		CBPerAppData perAppData = {};
		CommonConstantBuffers::PerAppData = CreateConstantBuffer(InGDI, &perAppData, sizeof(perAppData));
		CBPerFrameData perFrameData = {};
		CommonConstantBuffers::PerFrameData = CreateConstantBuffer(InGDI, &perFrameData, sizeof(perFrameData));
		CBPerObjectData perObjectData = {};
		CommonConstantBuffers::PerObjectData = CreateConstantBuffer(InGDI, &perObjectData, sizeof(perObjectData));
	}

	GfxResId RenderResourceHelper::CreateConstantBuffer(GfxGDI* InGDI, void* InData, i32 InSize)
	{
		GfxBuffer* constantBuffer = nullptr;

		GfxBufferDesc desc;
		desc.AccessFlags = GfxCpuAccessFlags::CpuAccess_Write;
		desc.Usage = GfxUsageFlags::Usage_Dynamic;
		desc.BindFlags = GfxBindFlags::Bind_ConstantBuffer;
		desc.ByteWidth = InSize;

		GfxBufferData data;
		data.Data = &InData;
		data.Size = InSize;

		return InGDI->CreateBuffer(desc, data, &constantBuffer);
	}

	void RenderResourceHelper::CreatePSODefaults(Application* InApplication)
	{
		auto gdi = InApplication->GetGDI();
		auto rs = InApplication->GetResourceSystem();

		// Create DEBUG PSO!
		GfxPipelineStateObjectDesc desc = {};
		desc.RasterizerState.CullMode = GfxCullMode::CullBack;
		desc.RasterizerState.FillMode = GfxFillMode::FillSolid;
		desc.RasterizerState.FrontCounterClockwise = 0;

		desc.DepthStencilState.DepthEnable = true;
		desc.DepthStencilState.DepthFunc = GfxComparisonFunc::LessEqual;
		desc.DepthStencilState.DepthWriteMask = GfxDepthWriteMask::DepthWriteMaskAll;
		
		desc.BlendState.RenderTarget[0].BlendEnable = FALSE;
		desc.BlendState.RenderTarget[0].RenderTargetWriteMask = (((1 | 2) | 4) | 8);

		desc.InputLayout = gPositionNormUV2Layout;
		desc.TopologyType = GfxTopologyType::TopologyPolygon;
		desc.PixelShaderId = rs->FindShader(CommonShaderHandles::DebugPS)->GfxShaderId;
		desc.VertexShaderId = rs->FindShader(CommonShaderHandles::DebugVS)->GfxShaderId;

		CachePSO("Debug", gdi->CreatePSO(desc, nullptr));

		// Create DEBUG PSO!
		GfxPipelineStateObjectDesc instanceDesc = {};
		instanceDesc.RasterizerState.CullMode = GfxCullMode::CullBack;
		instanceDesc.RasterizerState.FillMode = GfxFillMode::FillSolid;
		instanceDesc.RasterizerState.FrontCounterClockwise = 1;

		instanceDesc.DepthStencilState.DepthEnable = true;
		instanceDesc.DepthStencilState.DepthFunc = GfxComparisonFunc::LessEqual;
		instanceDesc.DepthStencilState.DepthWriteMask = GfxDepthWriteMask::DepthWriteMaskAll;

		instanceDesc.BlendState.RenderTarget[0].BlendEnable = FALSE;
		instanceDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = (((1 | 2) | 4) | 8);

		instanceDesc.InputLayout = gPositionNormUV2InstancedLayout;
		instanceDesc.TopologyType = GfxTopologyType::TopologyPolygon;
		instanceDesc.PixelShaderId = rs->FindShader(CommonShaderHandles::DebugPS)->GfxShaderId;
		instanceDesc.VertexShaderId = rs->FindShader(CommonShaderHandles::DebugInstancedVS)->GfxShaderId;

		CachePSO("DebugInstanced", gdi->CreatePSO(instanceDesc, nullptr));

		// Create Shadow PSO
		GfxPipelineStateObjectDesc shadowDesc = {};
		shadowDesc.RasterizerState.CullMode = GfxCullMode::CullFront;
		shadowDesc.RasterizerState.FillMode = GfxFillMode::FillSolid;
		shadowDesc.RasterizerState.FrontCounterClockwise = 0;

		shadowDesc.BlendState.RenderTarget[0].BlendEnable = FALSE;
		shadowDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = (((1 | 2) | 4) | 8);

		shadowDesc.InputLayout = gPositionNormUV2Layout;
		shadowDesc.TopologyType = GfxTopologyType::TopologyPolygon;
		shadowDesc.PixelShaderId = rs->FindShader(CommonShaderHandles::ColoredSimplePS)->GfxShaderId;
		shadowDesc.VertexShaderId = rs->FindShader(CommonShaderHandles::ShadowMapVS)->GfxShaderId;

		CachePSO("Shadow", gdi->CreatePSO(shadowDesc, nullptr));
	}

	void RenderResourceHelper::CachePSO(const std::string& InName, Dawn::GfxResId InId)
	{
		D_ASSERT(!CachedObjects.Exists(InName), std::string("PSO with the same name " + InName + " already added!").c_str());
		CachedObjects.Push(InName, InId);
	}

	Dawn::GfxResId RenderResourceHelper::GetCachedPSO(const std::string& InName)
	{
		D_ASSERT(CachedObjects.Exists(InName), std::string("Couldn't find PSO with name " + InName).c_str());
		return CachedObjects[InName];
	}
}