#pragma once


namespace Dawn
{
	enum DbgPrimitiveType
	{
		Quad,
		Line,
		Box, 
		Plane
	};

	namespace RenderDebugInterface 
	{
		void Box();
		void Line();
		void Quad();
		void Plane();
	}
}