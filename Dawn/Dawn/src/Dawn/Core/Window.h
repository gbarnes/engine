#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //
#define WIN32_LEAN_AND_MEAN
//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "inc_core.h"
#include <functional>
#include <Windows.h>

namespace Dawn
{
	//-----------------------------------------------------------------------------
	// Main Class Declarations
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	// Basic interface that provides necessary implementation details for       
	// any platform view within the project. Basically it is used to          
	// create windows for linux, windows or mac os which we can render to
	// at specific point.
	//-----------------------------------------------------------------------------
	class DAWN_API Window
	{

	public:
		std::function<void()> OnWindowPaint;
		std::function<void()> OnWindowProcHook;

		//-------------------------------------------------------------------------
		// Constructors & Destructors
		//-------------------------------------------------------------------------
		Window();
		~Window(void);

		//-------------------------------------------------------------------------
		// Public Virtual Methods
		//-------------------------------------------------------------------------
		virtual EResult Initialize(std::wstring InTitle, int InWidth, int InHeight, bool InIsFullscreen,
			int InColorBits, int InDepthBits, int InAlphaBits);
		virtual EResult Create(void);
		virtual bool PeekMessages(void);

		void ToggleFullscreen();

		virtual HWND GetHwnd(void)
		{
			return HWnd;
		}
	private:
		//-------------------------------------------------------------------------
		// Private Members
		//-------------------------------------------------------------------------
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

		//-------------------------------------------------------------------------
		// Private Methods
		//-------------------------------------------------------------------------
		//-------------------------------------------------------------------------
		// Static Private Methods
		//-------------------------------------------------------------------------
		static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	};
}