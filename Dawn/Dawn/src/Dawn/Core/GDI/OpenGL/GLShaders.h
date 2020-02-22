#pragma once
/*#include "inc_common.h"
#include "../GfxResource.h"

namespace Dawn
{
	class GLShader : public GfxShader
	{
	public:

		GLShader(GfxResId Id);
		~GLShader();

		bool AttachSource(GfxShaderType InType, const char* InShaderCode) override;
		void Bind() override;
		void Unbind() override;

	public:
		void SetInt(const std::string& Name, i32 Value) override;
		void SetFloat(const std::string& Name, float Value) override;
		void SetVec2(const std::string& Name, const vec2& Value) override;
		void SetVec3(const std::string& Name, const vec3& Value) override;
		void SetVec4(const std::string& Name, const vec4& Value) override;
		void SetMat4(const std::string& Name, const mat4& Value) override;
	private:
		u32 ProgramId;
		std::map<GfxShaderType, u32> ShaderIds;
	};
}*/