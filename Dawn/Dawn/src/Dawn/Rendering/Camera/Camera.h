#pragma once
#include "Core/GDI/inc_gfx_types.h"

namespace Dawn
{
	class CameraSystem;

	struct Camera
	{
		float FieldOfView = 65.0f;
		float NearZ = 100.0f;
		float FarZ = 0.1f;
		float AspectRatio = 0;
		Vec3 Position = { 0.0f, 0.0f, 0.0f };
		Vec3 LookAt = {0.0f, 0.0f, 0.0f};
		Vec3 Up = { 0.0f, 1.0f, 0.0f };

		const DirectX::XMMATRIX CalculateView()
		{
			return DirectX::XMMatrixLookAtLH(Position, LookAt, Up);
		}

		const DirectX::XMMATRIX CalculatePerspective()
		{
			return DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(FieldOfView), AspectRatio, NearZ, FarZ);
		}
	};
}