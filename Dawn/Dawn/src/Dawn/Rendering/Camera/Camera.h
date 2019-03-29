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

		Mat4 Projection;

		const Mat4 CalculateView()
		{
			return DirectX::XMMatrixLookAtLH(Position, LookAt, Up);
		}

		void CalculatePerspective()
		{
			
			Projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(FieldOfView), AspectRatio, NearZ, FarZ);
		}

		const Vec3 WorldToScreenPoint(Vec3 InPoint)
		{
			return DirectX::XMVector3Project(InPoint, 0, 0, 0, 0, 0, 1, Projection, CalculateView(), MAT4_IDENTITY);
		}

		const Vec3 ScreenToWorldPoint(Vec3 InPoint)
		{
			return DirectX::XMVector3Unproject(InPoint, 0, 0, 0, 0, 0, 1, Projection, CalculateView(), MAT4_IDENTITY);
		}
	};
}