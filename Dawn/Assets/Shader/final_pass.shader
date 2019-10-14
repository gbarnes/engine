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

out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
	FragColor = texture(screenTexture, TexCoords);
}