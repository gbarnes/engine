
Texture2D DiffuseTexture            : register(t0);
SamplerState Sampler    : register(s0);

struct PixelShaderInput
{
	float4 Position : SV_POSITION;
	float2 UV    : TEXCOORD0;
	float2 UV2      : TEXCOORD1;
};

float4 main(PixelShaderInput IN) : SV_TARGET
{
	return DiffuseTexture.Sample(Sampler, IN.UV) * DiffuseTexture.Sample(Sampler, IN.UV2);
}