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

out vec4 FragColor;

uniform sampler2D gPosition; 
uniform sampler2D gNormal;
uniform sampler2D gAlbedoAO;
uniform sampler2D gMetallicRoughness;
uniform sampler2D gSSAO; 
 
in vec2 TexCoords;

uniform Light lights[NR_LIGHTS];
uniform DirectionalLight directionalLight;
uniform vec3 viewPos;
uniform mat4 view;

void main()
{
    vec3 NormalVP = texture(gNormal, TexCoords).rgb;

	if(NormalVP == vec3(0,0,0)) 
		discard;
		
	vec3 Normal = (inverse(view) * texture(gNormal, TexCoords)).rgb;	
    vec3 WorldPos =  (inverse(view) * texture(gPosition, TexCoords)).rgb;
	vec3 Albedo = texture(gAlbedoAO, TexCoords).rgb;
	float AO = texture(gAlbedoAO, TexCoords).a;
    float Metallic = texture(gMetallicRoughness, TexCoords).r;
	float Roughness = texture(gMetallicRoughness, TexCoords).g;
	float SampledAO = texture(gSSAO, TexCoords).r;
	
	vec3 N = normalize(Normal);
	vec3 V = normalize(viewPos - WorldPos);  
	
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, Albedo, Metallic);
	
	vec3 Lo = vec3(0.0); 
	
	for(int i = 0; i < 1; ++i)
	{ 
		Light light = lights[i];

		vec3 L = normalize(light.position - WorldPos);
		vec3 H = normalize(V + L);
		
		float attenuation = CalculateAttenuation(WorldPos, light.position); 
		vec3 radiance = vec3(light.color) * light.intensity;
		
		float NDG = TrowbdrigeDistributionGGX(N, H, Roughness);
		float G = GeometrySmith(N, V, L, Roughness); 
		vec3 Fresnel = FresnelSchlick(max(dot(H, V), 0.0), F0);
		
		vec3 numerator = NDG * G * Fresnel;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		vec3 specular = numerator / max(denominator, 0.001);
		
		vec3 kS = Fresnel;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - Metallic;
		float NdotL = max(dot(N, L), 0.0);    
		
		Lo += (kD * Albedo / PI + specular) * radiance * NdotL;
	}
	
    vec3 ambient = vec3(0.03) * Albedo;
    vec3 color = ambient + Lo;
	
    color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2));
   
    FragColor = vec4(color * SampledAO, 1);
}