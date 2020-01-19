#version 330 core
#pragma vert_begin

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv0;
layout (location = 3) in vec2 uv1;
layout(location = 4) in mat4 modelInstanced;

uniform int isInstanced;
uniform mat4 lightSpace;
uniform mat4 model;

void main()
{
	gl_Position = lightSpace * model * vec4(aPos, 1.0);
}

#pragma frag_begin

void main()
{
	// gl_FragDepth = gl_FragCoord.z;
}