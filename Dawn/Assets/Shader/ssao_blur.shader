#version 330 core
#pragma vert_begin

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV0;

out vec2 TexCoords;


void main()
{
	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
    TexCoords = aUV0;
}
	
#pragma frag_begin
#pragma include "shader_globals.include"

layout (location = 0) out vec4 OutputSSAO;

uniform sampler2D gSSAO;
in vec2 TexCoords;

void main()
{
	vec2 texelSize = 1.0 / vec2(textureSize(gSSAO, 0));
	float result = 0.0;
	
	for(int x = -2; x < 2; ++x)
	{
		for(int y = -2; y < 2; ++y)
		{
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			result += texture(gSSAO, TexCoords + offset).r;
		}
	}
	
	OutputSSAO.r = result / (4.0 * 4.0);
}