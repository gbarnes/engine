#pragma once
#include "inc_common.h"
#include "glad.h"

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

namespace Dawn
{
	enum MouseBtn
	{
		MouseBtn_Left = VK_LBUTTON,
		MouseBtn_Right = VK_RBUTTON,
		MouseBtn_Middle = VK_MBUTTON, 
		MouseBtn_X1,
		MouseBtn_X2,
		MouseBtn_Num
	};

	// We take the same ids as the windows WinUser defined ones 
	// for now! 
	enum KeyCode
	{
		KeyCode_Backspace = 0x08,
		KeyCode_Tab = 0x09,
		KeyCode_Return = 0x0D,
		KeyCode_Shift = 0x10,
		KeyCode_Alt = 0x12,
		KeyCode_Control = 0x11,
		KeyCode_CapLock = 0x14,
		KeyCode_Esc = 0x1B,
		KeyCode_Space = 0x20,
		KeyCode_PageUp = 0x21,
		KeyCode_PageDown = 0x22,
		KeyCode_LeftArrow = 0x25,
		KeyCode_UpArrow = 0x26,
		KeyCode_RightArrow = 0x27,
		KeyCode_DownArrow = 0x28,
		KeyCode_Print = 0x2C,
		KeyCode_Insert = 0x2D,
		KeyCode_Delete = 0x2E,
		KeyCode_0 = 0x30,
		KeyCode_1 = 0x31,
		KeyCode_2 = 0x32,
		KeyCode_3 = 0x33,
		KeyCode_4 = 0x34,
		KeyCode_5 = 0x35,
		KeyCode_6 = 0x36,
		KeyCode_7 = 0x37,
		KeyCode_8 = 0x38,
		KeyCode_9 = 0x39,
		KeyCode_A = 0x41,
		KeyCode_B = 0x42,
		KeyCode_C = 0x43,
		KeyCode_D = 0x44,
		KeyCode_E = 0x45,
		KeyCode_F = 0x46,
		KeyCode_G = 0x47,
		KeyCode_H = 0x48,
		KeyCode_I = 0x49,
		KeyCode_J = 0x4A,
		KeyCode_K = 0x4B,
		KeyCode_L = 0x4C,
		KeyCode_M = 0x4D,
		KeyCode_N = 0x4E,
		KeyCode_O = 0x4F,
		KeyCode_P = 0x50,
		KeyCode_Q = 0x51,
		KeyCode_R = 0x52,
		KeyCode_S = 0x53,
		KeyCode_T = 0x54,
		KeyCode_U = 0x55,
		KeyCode_V = 0x56,
		KeyCode_W = 0x57,
		KeyCode_X = 0x58,
		KeyCode_Y = 0x59,
		KeyCode_Z = 0x5A,
		KeyCode_F1 = 0x70,
		KeyCode_F2 = 0x71,
		KeyCode_F3 = 0x72,
		KeyCode_F4 = 0x73,
		KeyCode_F5 = 0x74,
		KeyCode_F6 = 0x75,
		KeyCode_F7 = 0x76,
		KeyCode_F8 = 0x77,
		KeyCode_F9 = 0x78,
		KeyCode_F10 = 0x79,
		KeyCode_F11 = 0x7A,
		KeyCode_F12 = 0x7B,
		KeyCode_LeftShift = 0xA0,
		KeyCode_RightShift = 0xA1,
		KeyCode_RightCtrl = 0xA2,
		KeyCode_LeftCtrl = 0xA3
	};
	#define KEY_CODE_AMOUNT 69

	enum KeyState
	{
		Up,
		Down,
		Pressed
	};


	void InitInput(HINSTANCE InHinstance, HWND InHwnd, u32 InWidth, u32 InHeight);
	void UpdateInput();
	void ShutdownInput();
	bool IsKeyDown(KeyCode Code);
	bool IsKeyPressed(KeyCode InCode);
	bool IsMouseDown(MouseBtn InCode);
	vec2 GetMousePosition();
	
}