#pragma once
#include "Core/Event.h"

namespace Dawn 
{
	class UIFovChangedEvent : public Event
	{
	public:
		UIFovChangedEvent(float fov)
			: FOV(fov) {}

		inline float GetFov() const { return FOV; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "UIFovChangedEvent: " << FOV;
			return ss.str();
		}

	private:
		float FOV;
	};

	class UICamPosChangedEvent : public Event
	{
	public:
		UICamPosChangedEvent(float* pos)
			: Position(pos) {}

		inline float* GetPos() const { return &Position[0]; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "UICamPosChangedEvent: " << Position[0] << ", " << Position[1] << ", " << Position[3];
			return ss.str();
		}

	private:
		float* Position;
	};

	#define FOVChangedEvtKey EVENT_KEY("FOVChanged")
	#define CamPosChangedEvtKey EVENT_KEY("CamPosChanged")
}