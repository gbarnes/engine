#include "Window.h"
#include <windows.h>
#include <windowsx.h>
#include "Vendor/ImGui/ImGuiWrapper.h"
#include "imgui.h"
#include "Input.h"

namespace Dawn
{
	Window::Window()
	{
		this->Instance = nullptr;
		this->PrevInstance = nullptr;
		this->CmdLine = nullptr;
		this->CmdShow = 0;
		this->Title = L"Engine";
		this->IsFullscreen = false;
		this->ClassName = L"EngineClassWindow";
		this->IsInitialized = false;
		this->IsCreated = false;
		this->Width = 800;
		this->Height = 600;
	}

	EResult Window::Initialize(std::wstring InTitle, int InWidth, int InHeight, bool InIsFullscreen,
		int InColorBits, int InDepthBits, int InAlphaBits)
	{
		this->Title = InTitle;
		this->Width = InWidth;
		this->Height = InHeight;
		this->IsFullscreen = InIsFullscreen;
		this->ColorBits = InColorBits;
		this->DepthBits = InDepthBits;
		this->AlphaBits = InAlphaBits;

		// Windows 10 Creators update adds Per Monitor V2 DPI awareness context.
		// Using this awareness context allows the client area of the window 
		// to achieve 100% scaling while still allowing non-client window content to 
		// be rendered in a DPI sensitive fashion.
		SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

		// create the window class with necessary settings
		WNDCLASSEXW wndclass = {
								sizeof(WNDCLASSEXW),
								CS_DBLCLKS,
								Window::WndProc,
								0,
								0,
								GetModuleHandle(0),
								LoadIcon(0,IDI_APPLICATION),
								LoadCursor(0,IDC_ARROW),
								(HBRUSH)GetStockObject(BLACK_BRUSH),
								0,
								this->ClassName.c_str(),
								LoadIcon(0,IDI_APPLICATION)
		};
		this->WndClassEx = wndclass;

		this->Instance = wndclass.hInstance;

		// if we fail to successfully initialize the window ex class
		// please return an according error result.
		if (!RegisterClassExW(&this->WndClassEx))
		{
			return EResult_ERROR;
		}

		// once everything worked we can successfully say that 
		// we initialized the window!
		IsInitialized = true;

		return EResult_OK;
	}

	EResult Window::Create(void)
	{
		if (!IsInitialized)
			return EResult_ERROR;


		int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

		RECT windowRect = { 0, 0, static_cast<LONG>(Width), static_cast<LONG>(Height) };
		::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

		int windowWidth = windowRect.right - windowRect.left;
		int windowHeight = windowRect.bottom - windowRect.top;

		// Center the window within the screen. Clamp to 0, 0 for the top-left corner.
		int windowX = std::max<int>(0, (screenWidth - windowWidth) / 2);
		int windowY = std::max<int>(0, (screenHeight - windowHeight) / 2);

		this->HWnd = CreateWindowW(ClassName.c_str(),
			Title.c_str(),
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			windowX,
			windowY,
			windowWidth,
			windowHeight,
			NULL, NULL, Instance, NULL);

		// if anything went wrong we'd like to return an 
		// error thus we may be able to process it further
		if (!this->HWnd)
		{
			return EResult_ERROR;
		}

		SetProp(HWnd, L"Window", (HANDLE)this);

		//this->Instance = (HINSTANCE)GetWindowLong(HWnd, -6);

		ShowWindow(this->HWnd, 1);
		UpdateWindow(this->HWnd);
		IsCreated = true;


		return EResult_OK;
	}

	//
	//
	//
	void Window::ToggleFullscreen()
	{
		u32 NewWidth = 0;
		u32 NewHeight = 0;

		IsFullscreen = !IsFullscreen;
		if (IsFullscreen) // Switching to fullscreen.
		{
			// Store the current window dimensions so they can be restored 
			// when switching out of fullscreen state.
			::GetWindowRect(HWnd, &this->WindowRect);

			// Set the window style to a borderless window so the client area fills
			// the entire screen.
			UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
			::SetWindowLongW(HWnd, GWL_STYLE, windowStyle);

			// Query the name of the nearest display device for the window.
			// This is required to set the fullscreen dimensions of the window
			// when using a multi-monitor setup.
			HMONITOR hMonitor = ::MonitorFromWindow(HWnd, MONITOR_DEFAULTTONEAREST);
			MONITORINFOEX monitorInfo = {};
			monitorInfo.cbSize = sizeof(MONITORINFOEX);
			::GetMonitorInfo(hMonitor, &monitorInfo);

			::SetWindowPos(HWnd, HWND_TOP,
				monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.top,
				monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			NewWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
			NewHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

			::ShowWindow(HWnd, SW_MAXIMIZE);
		}
		else
		{
			// Restore all the window decorators.
			::SetWindowLong(HWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

			::SetWindowPos(HWnd, HWND_NOTOPMOST,
				WindowRect.left,
				WindowRect.top,
				WindowRect.right - WindowRect.left,
				WindowRect.bottom - WindowRect.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			NewWidth = WindowRect.right - WindowRect.left;
			NewHeight = WindowRect.bottom - WindowRect.top;

			::ShowWindow(HWnd, SW_NORMAL);
		}

		if (OnWindowResize != nullptr)
			OnWindowResize(NewWidth, NewHeight);
	}


	//-----------------------------------------------------------------------------
	// Peeks all the messages and takes care of dispatching them between the 
	// system. 
	//-----------------------------------------------------------------------------
	bool Window::PeekMessages(void)
	{
		if (PeekMessage(&this->MSG, NULL, NULL, NULL, PM_REMOVE))
		{
			if (this->MSG.message == WM_QUIT || this->MSG.message == WM_CLOSE)
			{
				return false;
			}


			TranslateMessage(&this->MSG);
			DispatchMessage(&this->MSG);
		}

		return true;
	}


	//-----------------------------------------------------------------------------
	// Handles the message pumping and takes care of successfully delivering 
	// the message to the window. 
	//-----------------------------------------------------------------------------
	LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (ImGuiWrapper::InputHandling(hwnd, message, wParam, lParam))
			return true;


		switch (message)
		{

		case WM_SIZE:
		{
			RECT clientRect = {};
			::GetClientRect(hwnd, &clientRect);

			int width = clientRect.right - clientRect.left;
			int height = clientRect.bottom - clientRect.top;

			Window* window = (Window*)GetProp(hwnd, L"Window");
			if (NULL != window)
			{
				if (window->OnWindowResize != nullptr)
					window->OnWindowResize(width, height);
			}

			// TODO resize method here!
			//ImGuiWrapper::Resize(); //DO we really need this?! 
			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		/*case WM_SYSCHAR: case WM_SYSKEYDOWN: case WM_SYSKEYUP:
		{
			bool alt = (::GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

			switch (wParam)
			{
			case VK_ESCAPE:
				::PostQuitMessage(0);
				break;
			case VK_RETURN:
				if (alt)
				{
				case VK_F11:
					Window* window = (Window*)GetProp(hwnd, L"Window");
					if (NULL != window)
					{
						window->ToggleFullscreen();
					}
				}
				break;
			}

			break;
		}*/

		/*case WM_PAINT:
		{
			Window* window = (Window*)GetProp(hwnd, L"Window");
			if (NULL != window)
			{
				if (window->OnWindowPaint != nullptr)
					window->OnWindowPaint();
			}
			
		} break;*/
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
			break;
		} 

		return 0;
	}
	
	Window::~Window(void)
	{
	}
}