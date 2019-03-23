cbuffer ModelViewProjection : register(b0)
{
	matrix MVP;
};

struct VertexPosUV
{
	float3 Position : POSITION;
	float2 UV    : TEXCOORD;
};

struct VertexShaderOutput
{
	float4 Position : SV_Position;
	float2 UV    : TEXCOORD;
};


VertexShaderOutput main(VertexPosUV IN)
{
	VertexShaderOutput OUT;

	OUT.Position = mul(MVP, float4(IN.Position, 1.0f));
	OUT.UV = IN.UV;

	return OUT;
}