
Texture2D DiffuseTexture            : register(t0);
SamplerState LinearRepeatSampler    : register(s0);

struct PixelShaderInput
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD;
};

float4 main(PixelShaderInput IN) : SV_TARGET
{
	return DiffuseTexture.Sample(LinearRepeatSampler, IN.UV);
}