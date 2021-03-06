struct PixelShaderInput
{
	float4 Position : SV_Position;
	float4 Color    : COLOR;
};

float4 main(PixelShaderInput IN) : SV_TARGET
{
	return IN.Color; 
}