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
	ResourceId CommonShaderHandles::GBufferFillPS;
	ResourceId CommonShaderHandles::GBufferFillVS;
	ResourceId CommonShaderHandles::GBufferFillInstancedVS;
	ResourceId CommonShaderHandles::FxaaPS;
	ResourceId CommonShaderHandles::FullscreenQuadVS;
	ResourceId CommonShaderHandles::PBRLightingPS;

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

		CommonShaderHandles::DebugInstancedVS = InResourceSystem->LoadFile("Shader/HLSL/debug_instanced.h_vs");
		CommonShaderHandles::DebugVS = InResourceSystem->LoadFile("Shader/HLSL/debug.h_vs");
		CommonShaderHandles::DebugPS = InResourceSystem->LoadFile("Shader/HLSL/debug.h_ps");

		CommonShaderHandles::ShadowMapVS = InResourceSystem->LoadFile("Shader/HLSL/shadowpass.h_vs");
		CommonShaderHandles::ColoredSimplePS = InResourceSystem->LoadFile("Shader/HLSL/colored_simple.h_ps");
		CommonShaderHandles::GBufferFillPS = InResourceSystem->LoadFile("Shader/HLSL/gbuffer_fill.h_ps");
		CommonShaderHandles::GBufferFillVS = InResourceSystem->LoadFile("Shader/HLSL/gbuffer_fill.h_vs");
		CommonShaderHandles::GBufferFillInstancedVS = InResourceSystem->LoadFile("Shader/HLSL/gbuffer_fill_instanced.h_vs");

		CommonShaderHandles::PBRLightingPS = InResourceSystem->LoadFile("Shader/HLSL/pbr_lighting.h_ps");
		CommonShaderHandles::FxaaPS = InResourceSystem->LoadFile("Shader/HLSL/fxaa_pass.h_ps");
		CommonShaderHandles::FullscreenQuadVS = InResourceSystem->LoadFile("Shader/HLSL/fullscreen_quad.h_vs");
	}

	GfxResId CommonConstantBuffers::PerAppData;
	GfxResId CommonConstantBuffers::PerObjectData;
	GfxResId CommonConstantBuffers::PerFrameData;
	GfxResId CommonConstantBuffers::MaterialData;
	GfxResId CommonConstantBuffers::PointLightData;
	GfxResId CommonConstantBuffers::DirLightData;

	std::unordered_map<std::string, Dawn::GfxResId> CachedObjects;

	void RenderResourceHelper::CreateConstantBuffers(GfxGDI* InGDI)
	{
		CBPerAppData perAppData = {};
		CommonConstantBuffers::PerAppData = CreateConstantBuffer(InGDI, &perAppData, sizeof(perAppData));
		CBPerFrameData perFrameData = {};
		CommonConstantBuffers::PerFrameData = CreateConstantBuffer(InGDI, &perFrameData, sizeof(perFrameData));
		CBPerObjectData perObjectData = {};
		CommonConstantBuffers::PerObjectData = CreateConstantBuffer(InGDI, &perObjectData, sizeof(perObjectData));
		CBMaterial materialData = {};
		CommonConstantBuffers::MaterialData = CreateConstantBuffer(InGDI, &materialData, sizeof(materialData));
		CBPointLightData lightData = {};
		CommonConstantBuffers::PointLightData = CreateConstantBuffer(InGDI, &lightData, sizeof(lightData));
		CBPointLightData dirLightData = {};
		CommonConstantBuffers::DirLightData = CreateConstantBuffer(InGDI, &dirLightData, sizeof(dirLightData));
	}

	GfxResId CommonSamplers::DefaultSampler;
	void RenderResourceHelper::CreateSamplers(GfxGDI* InGDI)
	{
		GfxSamplerDesc desc = {};
		desc.AddressU = GfxTextureAddressMode::Wrap;
		desc.AddressV = GfxTextureAddressMode::Wrap;
		desc.AddressW = GfxTextureAddressMode::Wrap;
		desc.Filter = GfxFilter::MinMagMipLinear;
		desc.MaxAnisotropy = 0;
		desc.MipLODBias = 0;

		CommonSamplers::DefaultSampler = InGDI->CreateSampler(desc, nullptr);
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

		// data driven?!
		// Create DEBUG PSO!
		GfxPipelineStateObjectDesc desc = {};
		desc.RasterizerState.CullMode = GfxCullMode::CullNone;
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
		instanceDesc.RasterizerState.CullMode = GfxCullMode::CullNone;
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
		shadowDesc.RasterizerState.CullMode = GfxCullMode::CullNone;
		shadowDesc.RasterizerState.FillMode = GfxFillMode::FillSolid;
		shadowDesc.RasterizerState.FrontCounterClockwise = 0;

		shadowDesc.BlendState.RenderTarget[0].BlendEnable = FALSE;
		shadowDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = (((1 | 2) | 4) | 8);

		shadowDesc.InputLayout = gPositionNormUV2Layout;
		shadowDesc.TopologyType = GfxTopologyType::TopologyPolygon;
		shadowDesc.PixelShaderId = rs->FindShader(CommonShaderHandles::ColoredSimplePS)->GfxShaderId;
		shadowDesc.VertexShaderId = rs->FindShader(CommonShaderHandles::ShadowMapVS)->GfxShaderId;

		CachePSO("Shadow", gdi->CreatePSO(shadowDesc, nullptr));

		// Create Geometry PSO
		GfxPipelineStateObjectDesc geometryPassDesc = {};
		geometryPassDesc.RasterizerState.CullMode = GfxCullMode::CullBack;
		geometryPassDesc.RasterizerState.FillMode = GfxFillMode::FillSolid;
		geometryPassDesc.RasterizerState.FrontCounterClockwise = 0;
		geometryPassDesc.RasterizerState.AntialiasedLineEnable = false;

		geometryPassDesc.DepthStencilState.DepthEnable = true;
		geometryPassDesc.DepthStencilState.DepthFunc = GfxComparisonFunc::LessEqual;
		geometryPassDesc.DepthStencilState.DepthWriteMask = GfxDepthWriteMask::DepthWriteMaskAll;

		geometryPassDesc.BlendState.RenderTarget[0].BlendEnable = FALSE;
		geometryPassDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = (((1 | 2) | 4) | 8);
		geometryPassDesc.BlendState.RenderTarget[1].BlendEnable = FALSE;
		geometryPassDesc.BlendState.RenderTarget[1].RenderTargetWriteMask = (((1 | 2) | 4) | 8);
		geometryPassDesc.BlendState.RenderTarget[2].BlendEnable = FALSE;
		geometryPassDesc.BlendState.RenderTarget[2].RenderTargetWriteMask = (((1 | 2) | 4) | 8);
		geometryPassDesc.BlendState.RenderTarget[3].BlendEnable = FALSE;
		geometryPassDesc.BlendState.RenderTarget[3].RenderTargetWriteMask = (((1 | 2) | 4) | 8);

		geometryPassDesc.InputLayout = gPositionNormUV2Layout;
		geometryPassDesc.TopologyType = GfxTopologyType::TopologyPolygon;
		geometryPassDesc.PixelShaderId = rs->FindShader(CommonShaderHandles::GBufferFillPS)->GfxShaderId;
		geometryPassDesc.VertexShaderId = rs->FindShader(CommonShaderHandles::GBufferFillVS)->GfxShaderId;

		CachePSO("GeometryPass", gdi->CreatePSO(geometryPassDesc, nullptr));

		// instead of doing this we should have functions to change the winding order http://www.dillonbhuff.com/?p=30
		geometryPassDesc.RasterizerState.FrontCounterClockwise = 0; 
		geometryPassDesc.InputLayout = gPositionNormUV2InstancedLayout;
		geometryPassDesc.VertexShaderId = rs->FindShader(CommonShaderHandles::GBufferFillInstancedVS)->GfxShaderId;
		CachePSO("GeometryPassInstanced", gdi->CreatePSO(geometryPassDesc, nullptr));

		// Create PBR-Pass PSO
		GfxPipelineStateObjectDesc pbrLightPassDesc = {};
		pbrLightPassDesc.RasterizerState.CullMode = GfxCullMode::CullBack;
		pbrLightPassDesc.RasterizerState.FillMode = GfxFillMode::FillSolid;
		pbrLightPassDesc.RasterizerState.FrontCounterClockwise = 0;

		pbrLightPassDesc.BlendState.RenderTarget[0].BlendEnable = FALSE;
		pbrLightPassDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = (((1 | 2) | 4) | 8);

		pbrLightPassDesc.InputLayout = gPositionUVLayout;
		pbrLightPassDesc.TopologyType = GfxTopologyType::TopologyPolygon;
		pbrLightPassDesc.PixelShaderId = rs->FindShader(CommonShaderHandles::PBRLightingPS)->GfxShaderId;
		pbrLightPassDesc.VertexShaderId = rs->FindShader(CommonShaderHandles::FullscreenQuadVS)->GfxShaderId;

		CachePSO("PBRLightingPass", gdi->CreatePSO(pbrLightPassDesc, nullptr));

		// Create FXAA-Pass PSO
		GfxPipelineStateObjectDesc fxaaPassDesc = {};
		fxaaPassDesc.RasterizerState.CullMode = GfxCullMode::CullBack;
		fxaaPassDesc.RasterizerState.FillMode = GfxFillMode::FillSolid;
		fxaaPassDesc.RasterizerState.FrontCounterClockwise = 0;

		fxaaPassDesc.BlendState.RenderTarget[0].BlendEnable = FALSE;
		fxaaPassDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = (((1 | 2) | 4) | 8);

		fxaaPassDesc.InputLayout = gPositionUVLayout;
		fxaaPassDesc.TopologyType = GfxTopologyType::TopologyPolygon;
		fxaaPassDesc.PixelShaderId = rs->FindShader(CommonShaderHandles::FxaaPS)->GfxShaderId;
		fxaaPassDesc.VertexShaderId = rs->FindShader(CommonShaderHandles::FullscreenQuadVS)->GfxShaderId;

		CachePSO("FXAAPass", gdi->CreatePSO(fxaaPassDesc, nullptr));
	}

	void RenderResourceHelper::CachePSO(const std::string& InName, Dawn::GfxResId InId)
	{
		D_ASSERT(CachedObjects.find(InName) == CachedObjects.end(), std::string("PSO with the same name " + InName + " already added!").c_str());
		CachedObjects.insert(std::pair(InName, InId));
	}

	Dawn::GfxResId RenderResourceHelper::GetCachedPSO(const std::string& InName)
	{
		D_ASSERT(CachedObjects.find(InName) != CachedObjects.end(), std::string("Couldn't find PSO with name " + InName).c_str());
		return CachedObjects[InName];
	}
}