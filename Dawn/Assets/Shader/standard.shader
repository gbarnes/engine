<?xml version="1.0"?>
<shader>
	<step type="vs"><![CDATA[#version 330 core

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
}]]>
	</step>
	<step type="ps"><![CDATA[#version 330 core

out vec4 FragColor;

in vec2 TexCoord0;
in vec2 TexCoord1;
in vec3 FragPos;  
in vec3 Normal;  

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
}; 
  
uniform Material material;

struct Light {
    vec3 position;
    vec4 diffuse;
};

uniform Light light;  

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);  
	float diff = max(dot(norm, lightDir), 0.0);
	vec4 diffuse = diff * light.diffuse;

	FragColor = diffuse * material.diffuse;
}]]>
	</step>
</shader>