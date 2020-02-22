#version 330 core
#pragma vert_begin
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv0;
layout(location = 3) in vec2 uv1;

out vec2 TexCoord0;
out vec2 TexCoord1;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	TexCoord0 = uv0;
	TexCoord1 = uv1;
}

#pragma frag_begin

out vec4 FragColor;

in vec2 TexCoord0;
in vec2 TexCoord1;

uniform sampler2D ourTexture;

void main()
{
	FragColor = texture(ourTexture, TexCoord0) * texture(ourTexture, TexCoord1);
}