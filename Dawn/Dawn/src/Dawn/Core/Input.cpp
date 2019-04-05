#include "Input.h"

namespace Dawn
{
	vec2 Input::MousePos;
	KeyState Input::MouseBtnMap[];
	std::map<KeyCode, KeyState> Input::KeyCodeMap;

	void Input::Reset()
	{
		KeyCodeMap[KeyCode_Backspace]   = KeyState::Up;
		KeyCodeMap[KeyCode_Tab]			= KeyState::Up;
		KeyCodeMap[KeyCode_Return]		= KeyState::Up;
		KeyCodeMap[KeyCode_Shift]		= KeyState::Up;
		KeyCodeMap[KeyCode_Alt]			= KeyState::Up;
		KeyCodeMap[KeyCode_Control]		= KeyState::Up;
		KeyCodeMap[KeyCode_CapLock]		= KeyState::Up;
		KeyCodeMap[KeyCode_Esc]			= KeyState::Up;
		KeyCodeMap[KeyCode_Space]		= KeyState::Up;
		KeyCodeMap[KeyCode_PageUp]		= KeyState::Up;
		KeyCodeMap[KeyCode_PageDown]	= KeyState::Up;
		KeyCodeMap[KeyCode_LeftArrow]	= KeyState::Up;
		KeyCodeMap[KeyCode_UpArrow]		= KeyState::Up;
		KeyCodeMap[KeyCode_RightArrow]	= KeyState::Up;
		KeyCodeMap[KeyCode_DownArrow]	= KeyState::Up;
		KeyCodeMap[KeyCode_Print]		= KeyState::Up;
		KeyCodeMap[KeyCode_Insert]		= KeyState::Up;
		KeyCodeMap[KeyCode_Delete]		= KeyState::Up;
		KeyCodeMap[KeyCode_0]			= KeyState::Up;
		KeyCodeMap[KeyCode_1]			= KeyState::Up;
		KeyCodeMap[KeyCode_2]			= KeyState::Up;
		KeyCodeMap[KeyCode_3]			= KeyState::Up;
		KeyCodeMap[KeyCode_4]			= KeyState::Up;
		KeyCodeMap[KeyCode_5]			= KeyState::Up;
		KeyCodeMap[KeyCode_6]			= KeyState::Up;
		KeyCodeMap[KeyCode_7]			= KeyState::Up;
		KeyCodeMap[KeyCode_8]			= KeyState::Up;
		KeyCodeMap[KeyCode_9]			= KeyState::Up;
		KeyCodeMap[KeyCode_A]			= KeyState::Up;
		KeyCodeMap[KeyCode_B]			= KeyState::Up;
		KeyCodeMap[KeyCode_C]			= KeyState::Up;
		KeyCodeMap[KeyCode_D]			= KeyState::Up;
		KeyCodeMap[KeyCode_E]			= KeyState::Up;
		KeyCodeMap[KeyCode_F]			= KeyState::Up;
		KeyCodeMap[KeyCode_G]			= KeyState::Up;
		KeyCodeMap[KeyCode_H]			= KeyState::Up;
		KeyCodeMap[KeyCode_I]			= KeyState::Up;
		KeyCodeMap[KeyCode_J]			= KeyState::Up;
		KeyCodeMap[KeyCode_K]			= KeyState::Up;
		KeyCodeMap[KeyCode_L]			= KeyState::Up;
		KeyCodeMap[KeyCode_M]			= KeyState::Up;
		KeyCodeMap[KeyCode_N]			= KeyState::Up;
		KeyCodeMap[KeyCode_O]			= KeyState::Up;
		KeyCodeMap[KeyCode_P]			= KeyState::Up;
		KeyCodeMap[KeyCode_Q]			= KeyState::Up;
		KeyCodeMap[KeyCode_R]			= KeyState::Up;
		KeyCodeMap[KeyCode_S]			= KeyState::Up;
		KeyCodeMap[KeyCode_T]			= KeyState::Up;
		KeyCodeMap[KeyCode_U]			= KeyState::Up;
		KeyCodeMap[KeyCode_V]			= KeyState::Up;
		KeyCodeMap[KeyCode_W]			= KeyState::Up;
		KeyCodeMap[KeyCode_X]			= KeyState::Up;
		KeyCodeMap[KeyCode_Y]			= KeyState::Up;
		KeyCodeMap[KeyCode_Z]			= KeyState::Up;
		KeyCodeMap[KeyCode_F1]			= KeyState::Up;
		KeyCodeMap[KeyCode_F2]			= KeyState::Up;
		KeyCodeMap[KeyCode_F3]			= KeyState::Up;
		KeyCodeMap[KeyCode_F4]			= KeyState::Up;
		KeyCodeMap[KeyCode_F5]			= KeyState::Up;
		KeyCodeMap[KeyCode_F6]			= KeyState::Up;
		KeyCodeMap[KeyCode_F7]			= KeyState::Up;
		KeyCodeMap[KeyCode_F8]			= KeyState::Up;
		KeyCodeMap[KeyCode_F9]			= KeyState::Up;
		KeyCodeMap[KeyCode_F10]			= KeyState::Up;
		KeyCodeMap[KeyCode_F11]			= KeyState::Up;
		KeyCodeMap[KeyCode_F12]			= KeyState::Up;
		KeyCodeMap[KeyCode_LeftShift]	= KeyState::Up;
		KeyCodeMap[KeyCode_RightShift]	= KeyState::Up;
		KeyCodeMap[KeyCode_RightCtrl]	= KeyState::Up;
		KeyCodeMap[KeyCode_LeftCtrl]	= KeyState::Up;
	}

}