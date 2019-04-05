<?xml version="1.0"?>
<shader>
	<step type="vs"><![CDATA[#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV0;

out vec2 TexCoord0;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	TexCoord0 = aUV0;
}]]>
	</step>
	<step type="ps"><![CDATA[#version 330 core

out vec4 FragColor;

in vec2 TexCoord0;

uniform sampler2D gridTexture;

void main()
{
	vec4 texColor = texture(gridTexture, TexCoord0);
	
	/*if(texColor.x < 0.1f && texColor.y < 0.1f && texColor.z < 0.1f )
	{
		FragColor = vec4(1.0f, 0.0f, 0.0f, 0.5f);
	}
	else {*/
		FragColor = texColor;
	//}
}]]>
	</step>
</shader>