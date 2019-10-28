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

layout (location = 0) out vec4 gSSAO;

uniform sampler2D gPosition; 
uniform sampler2D gNormal;
uniform sampler2D gAlbedo; 
uniform sampler2D Noise;

uniform vec3 samples[64];
uniform mat4 projection; 
uniform mat4 view;
uniform float radius;
uniform float bias;
uniform float power;
 
const vec2 noiseScale = vec2(1920.0/4.0, 1080.0/4.0); 
in vec2 TexCoords;

int kernelSize = 64;

void main() 
{
	vec3 fragPos = texture(gPosition, TexCoords).xyz;
    vec3 normal =  normalize(texture(gNormal, TexCoords).rgb);
    vec3 randomVec = texture(Noise, TexCoords * noiseScale).xyz;

    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
	
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
	
    for(int i = 0; i < kernelSize; ++i) 
    {
        // get sample position
		vec3 sample = TBN * samples[i];
        vec3 samplePos = fragPos + sample; // from tangent to view-space
        samplePos = fragPos + sample * radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + vec3(0.5); // transform to range 0.0 - 1.0

        // get sample depth
		float sampleDepth = (texture(gPosition, offset.xy).b); // get depth value of kernel sample
		
		// range check & accumulate
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck  * power; 	
    }

    gSSAO.r = 1.0 - (occlusion / kernelSize);
}