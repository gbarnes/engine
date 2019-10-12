#version 330 core
#pragma vert_begin

layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
}
	
#pragma frag_begin

out vec4 FragColor;

uniform vec4 color;

void main()
{
	FragColor = color;
}