#pragma once
#include "GfxResource.h"
#include "inc_common.h"

namespace Dawn
{
	typedef GenericHandle GfxShaderHandle;

	class GfxShader : public GfxResource
	{
	public:
		GfxShader(GfxShaderHandle InId) : Id(InId) {};
		virtual ~GfxShader();

		virtual void Create(const char* InShaderCode) = 0;

		inline const GfxShaderHandle GetId() const {
			return Id;
		}

	protected:
		GfxShaderHandle Id;
	};

	class GfxVertexShader : public GfxShader
	{
	};

	class GfxFragShader : public GfxShader
	{
	};
}