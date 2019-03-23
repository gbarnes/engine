#pragma once

#include <DirectXMath.h>

struct VertexPosColor
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Color;
};

struct VertexPosUV
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 UV;
};

struct VertexPosNormalUV
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 UV;
};