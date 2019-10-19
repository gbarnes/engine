#version 330 core

#pragma vert_begin
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 3) in vec2 uv0;
layout(location = 4) in vec2 uv1;

out vec2 TexCoord0;
out vec2 TexCoord1; 

out vec3 FragPos;  
out vec3 Normal;
 
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;  
 
void main()
{ 
	vec4 worldPos = model * vec4(position, 1.0);
	FragPos = worldPos.xyz;  
	TexCoord0 = uv0;  
	TexCoord1 = uv1; 
	
	mat3 normalMatrix = transpose(mat3(model));
    Normal = normalMatrix * normal;

	gl_Position = proj * view * worldPos;
}

#pragma frag_begin
#pragma include "shader_globals.include"
#pragma include "shader_gbuffer.include"


in vec2 TexCoord0;
in vec2 TexCoord1; 
in vec3 FragPos;  
in vec3 Normal;   

uniform Material material;

void main()
{
	FillGBuffer(material, FragPos, Normal); 
}