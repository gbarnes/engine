cbuffer ModelViewProjection : register(b0)
{
	matrix MVP;
};

//ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);


struct VertexPosColor
{
	float3 Position : POSITION;
	float3 Color    : COLOR;
};

struct VertexShaderOutput
{
	float4 Position : SV_POSITION;
	float4 Color    : COLOR;
};

VertexShaderOutput main(VertexPosColor IN)
{
	VertexShaderOutput OUT;

	OUT.Position = mul(MVP, float4(IN.Position, 1.0f));
	OUT.Color = float4(IN.Color, 1.0f);

	return OUT;
}