//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //


//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "../../inc_common.h"
#include "Window.h"
#include <windows.h>
#include <windowsx.h>


///////////////////////////////////////////////////////////////////////////////
// CWindow Member Definitions
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// Creates a new instance of the win32 view. 
//-----------------------------------------------------------------------------
CWindow::CWindow()
{
	this->Instance = nullptr;
	this->PrevInstance = nullptr;
	this->CmdLine = nullptr;
	this->CmdShow = 1;
	this->Title = L"Engine";
	this->IsFullscreen = false;
	this->ClassName = L"EngineClassWindow";
	this->IsInitialized = false;
	this->IsCreated = false;
	this->Width = 800;
	this->Height = 600;
}



//-----------------------------------------------------------------------------
// Initializes the Win32 Window with WNDCLASSEX and registers the window 
// properties, but in order to create the window you need to call VCreate().
// Method will return SKY_ERROR in case of any  error or SKY_OK in case 
// everything was ok. 
//-----------------------------------------------------------------------------
EResult CWindow::Initialize (std::wstring InTitle, int InWidth, int InHeight, bool InIsFullscreen,
								int InColorBits, int InDepthBits, int InAlphaBits)
{
	this->Title = InTitle;
	this->Width = InWidth;
	this->Height = InHeight;
	this->IsFullscreen = InIsFullscreen;
	this->ColorBits = InColorBits;
	this->DepthBits = InDepthBits;
	this->AlphaBits = InAlphaBits;

	// create the window class with necessary settings
	WNDCLASSEXW wndclass = { 
							sizeof(WNDCLASSEXW), 
							CS_DBLCLKS, 
							CWindow::WndProc,
							0, 
							0, 
							GetModuleHandle(0), 
							LoadIcon(0,IDI_APPLICATION),
							LoadCursor(0,IDC_ARROW), 
							(HBRUSH) GetStockObject ( BLACK_BRUSH ),
							0, 
							this->ClassName.c_str(),
							LoadIcon(0,IDI_APPLICATION) 
						  };
	this->WndClassEx = wndclass;

	// if we fail to successfully initialize the window ex class
	// please return an according error result.
	if( !RegisterClassExW( &this->WndClassEx ) ) 
	{
		return EResult_ERROR;
	}

	// once everything worked we can successfully say that 
	// we initialized the window!
	IsInitialized = true;

	return EResult_OK;
}

//-----------------------------------------------------------------------------
// Finally creates the window and the opengl context. This method returns
// SKY_ERROR if something goes wrong. 
//-----------------------------------------------------------------------------
EResult CWindow::Create (void)
{
	if(!IsInitialized)
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

	this->HWnd = CreateWindowW ( ClassName.c_str(), 
								 Title.c_str() , 
								WS_OVERLAPPEDWINDOW | WS_VISIBLE,
								windowX,
								windowY, 
								Width, 
								Height, 
								NULL, NULL, Instance, NULL);

	// if anything went wrong we'd like to return an 
	// error thus we may be able to process it further
	if( !this->HWnd ) 
	{
		return EResult_ERROR;
	}

	SetProp(HWnd, L"CWindow", (HANDLE)this);
	ShowWindow ( this->HWnd, this->CmdShow );
	UpdateWindow ( this->HWnd ) ;
	IsCreated = true;


	return EResult_OK;
}


//-----------------------------------------------------------------------------
// Peeks all the messages and takes care of dispatching them between the 
// system. 
//-----------------------------------------------------------------------------
bool CWindow::PeekMessages (void)
{
	if( PeekMessage ( &this->MSG, NULL, NULL, NULL, PM_REMOVE) ) 
	{
		if( this->MSG.message == WM_QUIT || this->MSG.message ==WM_CLOSE ) 
		{
			return true;
		}


		TranslateMessage( &this->MSG );
		DispatchMessage( &this->MSG );
	}
	 
	return false;
}


//-----------------------------------------------------------------------------
// Handles the message pumping and takes care of successfully delivering 
// the message to the window. 
//-----------------------------------------------------------------------------
LRESULT CALLBACK CWindow::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_PAINT:
			{
				CWindow* Window = (CWindow*)GetProp(hwnd, L"CWindow");
				if (NULL != Window)
				{
					Window->OnWindowPaint();
				}
				break;
			}
		case WM_SIZE: 
			{
				break;
			}
		case WM_DESTROY: 
			{
				PostQuitMessage(0);
				break;
			}
		case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
			{
				break;
			}
		case WM_CHAR:
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_SYSCHAR:
			{
				break;
			}
		default: 
			{
				return DefWindowProc(hwnd, message, wParam, lParam);
				break;
			}
	}
	
	return 0;
}

//-----------------------------------------------------------------------------
// Takes care of successfully freeing all resources!
//-----------------------------------------------------------------------------
CWindow::~CWindow(void)
{
	//SKY_PRINT_CONSOLE("skyDXWin32View:", "destructing object..", 0x0D);
	//SKY_INFO(skyLogChannel::TXT_FILE, "skyDXWin32View: destructing object..");
}