cbuffer ModelViewProjection : register(b0)
{
	matrix MVP;
};

struct VertexPosUV
{
	float3 Position : POSITION;
	float2 UV    : TEXCOORD0;
	float2 UV2      : TEXCOORD1;
};

struct VertexShaderOutput
{
	float4 Position : SV_Position;
	float2 UV    : TEXCOORD0;
	float2 UV2      : TEXCOORD1;
};


VertexShaderOutput main(VertexPosUV IN)
{
	VertexShaderOutput OUT;

	OUT.Position = mul(MVP, float4(IN.Position, 1.0f));
	OUT.UV = IN.UV;
	OUT.UV2 = IN.UV2;
	return OUT;
}