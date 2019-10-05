#pragma once

#define WIN32_LEAN_AND_MEAN

#include "inc_core.h"
#include <functional>
#include <Windows.h>

namespace Dawn
{
	class DAWN_API Window
	{

	public:
		std::function<void()> OnWindowPaint;
		std::function<void()> OnWindowProcHook;
		std::function<void(int, int)> OnWindowResize;

		Window();
		~Window(void);

		virtual EResult Initialize(std::wstring InTitle, int InWidth, int InHeight, bool InIsFullscreen,
			int InColorBits, int InDepthBits, int InAlphaBits);
		virtual EResult Create(void);
		virtual bool PeekMessages(void);

		void ToggleFullscreen();

		virtual HWND GetHwnd(void) const
		{
			return HWnd;
		}
	private:
		HINSTANCE Instance;
		HINSTANCE PrevInstance;
		HWND HWnd;
		MSG MSG;
		LPWSTR CmdLine;
		std::wstring ClassName;
		WNDCLASSEXW WndClassEx;

		std::wstring Title;

		RECT WindowRect;
		int Width;
		int Height;
		int CmdShow;
		int Format;
		int ColorBits;
		int DepthBits;
		int AlphaBits;

		bool IsFullscreen;
		bool IsInitialized;
		bool IsCreated;

		static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	};
}