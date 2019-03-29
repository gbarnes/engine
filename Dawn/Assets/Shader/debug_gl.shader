<?xml version="1.0"?>
<shader>
	<step type="vs"><![CDATA[#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV0;
layout(location = 2) in vec2 aUV1;

out vec2 TexCoord0;
out vec2 TexCoord1;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	TexCoord0 = aUV0;
	TexCoord1 = aUV1;
}]]>
	</step>
	<step type="ps"><![CDATA[#version 330 core

out vec4 FragColor;

in vec2 TexCoord0;
in vec2 TexCoord1;

uniform sampler2D ourTexture;

void main()
{
	FragColor = texture(ourTexture, TexCoord0) * texture(ourTexture, TexCoord1);
}]]>
	</step>
</shader>