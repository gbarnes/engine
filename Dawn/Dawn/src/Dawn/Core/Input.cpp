#include "Input.h"

namespace Dawn
{
	vec2 Input::MousePos;
	KeyState Input::MouseBtnMap[];
	KeyState Input::KeyCodeMap[];

	void Input::Reset()
	{
		for (u32 i = 0; i < MouseBtn::MouseBtn_Num; ++i)
			MouseBtnMap[i] = KeyState::Up;

		for (u32 i = 0; i < KEY_CODE_AMOUNT; ++i)
			KeyCodeMap[i] = KeyState::Up;
	}

}