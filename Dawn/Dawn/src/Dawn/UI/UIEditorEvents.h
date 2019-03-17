#pragma once

#include "inc_core.h"

namespace Dawn 
{
	class CUIFovChangedEvent : public CEvent
	{
	public:
		CUIFovChangedEvent(float fov)
			: FOV(fov) {}

		inline float GetFov() const { return FOV; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "CUIFovChangedEvent: " << FOV;
			return ss.str();
		}

	private:
		float FOV;
	};

	class CUICamPosChangedEvent : public CEvent
	{
	public:
		CUICamPosChangedEvent(float* pos)
			: Position(pos) {}

		inline float* GetPos() const { return &Position[0]; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "CUIFovChangedEvent: " << Position[0] << ", " << Position[1] << ", " << Position[3];
			return ss.str();
		}

	private:
		float* Position;
	};

	#define FOVChangedEvtKey EVENT_KEY("FOVChanged")
	#define CamPosChangedEvtKey EVENT_KEY("CamPosChanged")
}