#include "Input.h"

namespace Dawn
{
	struct InputObjects
	{
		HWND Hwnd;
	};

	struct InputData
	{
		unsigned char KeyboardStates[256];
		DIMOUSESTATE MouseState;
		u32 Width, Height;
		vec2 MousePos;
	};

	InputData g_InputData;
	InputObjects g_InputObjects;
	bool g_bIsInitialized = false;

	void ReadKeyboard()
	{
		GetKeyboardState((PBYTE)&g_InputData.KeyboardStates);
	}

	void ReadMouse()
	{
		POINT Point;
		if (GetCursorPos(&Point))
		{
			g_InputData.MousePos.x = Point.x;
			g_InputData.MousePos.y = Point.y;
		}
	}


	void InitInput(HINSTANCE InHinstance, HWND InHwnd, u32 InWidth, u32 InHeight)
	{
		if (g_bIsInitialized)
			return;

		g_InputData = {};
		g_InputObjects.Hwnd = InHwnd;

		g_InputData.Width = InWidth; 
		g_InputData.Height = InHeight;

		g_bIsInitialized = true;
	}

	void UpdateInput()
	{
		ReadKeyboard();
		ReadMouse();
	}

	void ShutdownInput()
	{
		g_InputObjects = {};
		g_InputData = {};
		g_bIsInitialized = false;
	}

	bool IsKeyDown(KeyCode Code)
	{
		return ((g_InputData.KeyboardStates[Code] & 0x80) != 0);
	}
	
	bool IsMouseDown(MouseBtn InCode)
	{
		return ((GetAsyncKeyState(InCode)) != 0);
	}

	vec2 GetMousePosition()
	{
		return g_InputData.MousePos;
	}
}