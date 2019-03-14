#pragma once

#include "inc_core.h"

namespace Dawn 
{
	class DAWN_API CMouseMovedEvent : public CEvent
	{
	public:
		CMouseMovedEvent(float x, float y)
			: MouseX(x), MouseY(y) {}

		inline float GetX() const { return MouseX; }
		inline float GetY() const { return MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << MouseX << ", " << MouseY;
			return ss.str();
		}

	private:
		float MouseX, MouseY;
	};
}