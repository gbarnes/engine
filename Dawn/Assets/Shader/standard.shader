#version 330 core

#pragma vert_begin
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv0;
layout(location = 3) in vec2 uv1;

out vec2 TexCoord0;
out vec2 TexCoord1;

out vec3 FragPos;  
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{ 
	gl_Position = projection * view * model * vec4(position, 1.0f);
	TexCoord0 = uv0; 
	TexCoord1 = uv1; 
	FragPos = vec3(model * vec4(position, 1.0));
	Normal = normal;
}

#pragma frag_begin
#pragma include "shader_globals.include"

out vec4 FragColor;

in vec2 TexCoord0;
in vec2 TexCoord1; 
in vec3 FragPos;  
in vec3 Normal;   

uniform Material material;
uniform Light light;  

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);  
	float diff = max(dot(norm, lightDir), 0.0); 
	vec4 diffuse = diff * light.diffuse; 
 
	FragColor = (vec4(0.35, 0.35, 0.35, 1.0) + diffuse) * material.diffuse;
}