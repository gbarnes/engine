#pragma once

#include "inc_core.h"

namespace Dawn 
{
	class DAWN_API MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y)
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


	class DAWN_API MousePressedEvent : public Event
	{
	public:
		MousePressedEvent(u32 InButton)
			: Button(InButton) {}

		inline u32 GetButton() const { return Button; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MousePressedEvent: " << Button;
			return ss.str();
		}

	private:
		u32 Button;
	};

	class DAWN_API MouseReleasedEvent : public Event
	{
	public:
		MouseReleasedEvent(u32 InButton)
			: Button(InButton) {}

		inline u32 GetButton() const { return Button; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseReleasedEvent: " << Button;
			return ss.str();
		}

	private:
		u32 Button;
	};

	
}