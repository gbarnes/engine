#include "stdafx.h"
#include "PSOCache.h"
#include "RenderResourceHelper.h"
#include "Core/GDI/Base/GfxInputLayouts.h"
#include "Core/GDI/Base/GfxPipelineStateObject.h"
#include "Core/GDI/Base/inc_gfx_types.h"
#include "ResourceSystem/ResourceSystem.h"
#include "Application.h"

Dawn::Map<std::string, Dawn::GfxResId> Dawn::PSOCache::CachedObjects;

void Dawn::PSOCache::CreateDefaults(Application* InApplication)
{
	auto gdi = InApplication->GetGDI();
	auto rs = InApplication->GetResourceSystem();

	// Create DEBUG PSO!
	GfxPipelineStateObjectDesc desc = {};
	desc.RasterizerState.CullMode = GfxCullMode::CullBack;
	desc.RasterizerState.FillMode = GfxFillMode::FillSolid; 
	desc.RasterizerState.FrontCounterClockwise = 1;

	desc.BlendState.RenderTarget[0].BlendEnable = FALSE;
	desc.BlendState.RenderTarget[0].RenderTargetWriteMask = (((1 | 2) | 4) | 8);

	desc.InputLayout = gPositionNormUV2Layout;
	desc.TopologyType = GfxTopologyType::TopologyPolygon;
	desc.PixelShaderId = rs->FindShader(CommonShaderHandles::DebugPS)->GfxShaderId;
	desc.VertexShaderId = rs->FindShader(CommonShaderHandles::DebugVS)->GfxShaderId;

	Cache("Debug", gdi->CreatePSO(desc, nullptr));


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

	Cache("Shadow", gdi->CreatePSO(shadowDesc, nullptr));
}

void Dawn::PSOCache::Cache(const std::string& InName, Dawn::GfxResId InId)
{
	D_ASSERT(!CachedObjects.Exists(InName), std::string("PSO with the same name " + InName + " already added!").c_str());
	CachedObjects.Push(InName, InId);
}

Dawn::GfxResId Dawn::PSOCache::GetCached(const std::string& InName)
{
	D_ASSERT(CachedObjects.Exists(InName), std::string("Couldn't find PSO with name " + InName).c_str());
	return CachedObjects[InName];
}