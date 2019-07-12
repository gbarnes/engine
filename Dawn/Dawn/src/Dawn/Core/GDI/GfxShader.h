#pragma once
#include "GfxResource.h"
#include "inc_common.h"

namespace Dawn
{
	class GfxShader : public GfxResource
	{
	public:
		GfxShader(GfxResId InId) : GfxResource(InId) {};
		virtual ~GfxShader() = default;

		virtual void Create(const char* InShaderCode) = 0;

		inline const GfxResId GetId() const {
			return Id;
		}
	};

	class GfxVertexShader : public GfxShader
	{
	};

	class GfxFragShader : public GfxShader
	{
	};
}