#pragma once
#include "inc_gfx_types.h"

namespace Dawn
{
	const GfxInputLayout gPositionColorLayout = 
	{
		{ GfxShaderDataType::Position, "position" },
		{ GfxShaderDataType::Color, "color" }
	};

	const GfxInputLayout gPositionUVLayout =
	{
		{ GfxShaderDataType::Position, "position" },
		{ GfxShaderDataType::TexCoord0, "uv" }
	};

	const GfxInputLayout gPositionNormUVLayout =
	{
		{ GfxShaderDataType::Position, "position" },
		{ GfxShaderDataType::Normal, "normal" },
		{ GfxShaderDataType::TexCoord0, "uv" }
	};

	const GfxInputLayout gPositionNormUV2Layout =
	{
		{ GfxShaderDataType::Position, "position" },
		{ GfxShaderDataType::Normal, "normal" },
		{ GfxShaderDataType::TexCoord0, "uv" },
		{ GfxShaderDataType::TexCoord1, "uv1" }
	};

	const GfxInputLayout gPositionNormUV2InstancedLayout =
	{
		{ GfxShaderDataType::Position, "position" },
		{ GfxShaderDataType::Normal, "normal" },
		{ GfxShaderDataType::TexCoord0, "uv" },
		{ GfxShaderDataType::TexCoord1, "uv1" },
		{ GfxShaderDataType::Matrix, "matrix", false, true }
	};
}