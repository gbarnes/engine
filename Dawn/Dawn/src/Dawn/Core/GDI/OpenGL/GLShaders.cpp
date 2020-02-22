#include "stdafx.h"
#include "GLShaders.h"
/*#include "Core/Logging/Log.h"
#include <glad.h>

namespace Dawn
{
	int ShaderTypeToGLCode(GfxShaderType InType)
	{
		if (InType == GfxShaderType::ST_Pixel)
			return GL_FRAGMENT_SHADER;
		else if (InType == GfxShaderType::ST_Vertex)
			return GL_VERTEX_SHADER;
		else if (InType == GfxShaderType::ST_Compute)
			return GL_COMPUTE_SHADER;

		return -1;
	}

	GLShader::GLShader(GfxResId Id)
		: GfxShader(Id)
	{
		ProgramId = glCreateProgram();
	}

	GLShader::~GLShader()
	{
		glDeleteProgram(ProgramId);
	}

	bool GLShader::AttachSource(GfxShaderType InType, const char* InShaderCode)
	{
		auto glCode = ShaderTypeToGLCode(InType);

		auto ShaderIdFound = ShaderIds.find(InType);
		if (ShaderIdFound != ShaderIds.end())
		{
			glDetachShader(ProgramId, ShaderIdFound->second);
			ShaderIds.erase(InType);
		}


		auto shaderId = glCreateShader(glCode);
		glShaderSource(shaderId, 1, &InShaderCode, NULL);
		glCompileShader(shaderId);

		int  success;
		char infoLog[512];
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
			DWN_CORE_ERROR("ERROR::SHADER::COMPILATION_FAILED {0} \n", std::string(infoLog));
			glDeleteShader(shaderId);
			return false;
		}

		glAttachShader(ProgramId, shaderId);
		glLinkProgram(ProgramId);
		ShaderIds.insert(std::make_pair(InType, shaderId));

		return true;
	}

	void GLShader::Bind()
	{
		glUseProgram(ProgramId);
	}

	void GLShader::Unbind()
	{
		glUseProgram(0);
	}

	void GLShader::SetInt(const std::string& Name, i32 Value)
	{
		glUniform1i(glGetUniformLocation(ProgramId, Name.c_str()), Value);
	}

	void GLShader::SetFloat(const std::string& Name, float Value)
	{
		glUniform1f(glGetUniformLocation(ProgramId, Name.c_str()), Value);
	}

	void GLShader::SetVec2(const std::string& Name, const vec2& Value)
	{
		glUniform2f(glGetUniformLocation(ProgramId, Name.c_str()), Value.x, Value.y);
	}

	void GLShader::SetVec3(const std::string& Name, const vec3& Value)
	{
		glUniform3f(glGetUniformLocation(ProgramId, Name.c_str()), Value.x, Value.y, Value.z);
	}

	void GLShader::SetVec4(const std::string& Name, const vec4& Value)
	{
		glUniform4f(glGetUniformLocation(ProgramId, Name.c_str()), Value.r, Value.g, Value.b, Value.a);
	}

	void GLShader::SetMat4(const std::string& Name, const mat4& Value)
	{
		glUniformMatrix4fv(glGetUniformLocation(ProgramId, Name.c_str()), 1, GL_FALSE, &Value[0][0]);
	}
}*/