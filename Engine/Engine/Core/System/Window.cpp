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
	WNDCLASSEXW wndclass = { sizeof(WNDCLASSEXW), 
							CS_DBLCLKS, CWindow::WndProc,
							0, 0, 
							GetModuleHandle(0), LoadIcon(0,IDI_APPLICATION),
							LoadCursor(0,IDC_ARROW), (HBRUSH) GetStockObject ( BLACK_BRUSH ),
							0, this->ClassName.c_str(),
							LoadIcon(0,IDI_APPLICATION) 
						  };

	this->WndClassEx = wndclass;

	// if we fail to successfully initialize the window ex class
	// please return an according error result.
	if( !RegisterClassExW( &this->WndClassEx ) ) 
	{
		//SKY_PRINT_CONSOLE("skyDXWin32View", "while creating the win32 window something went wrong! Couldn't register ClassEx. Aborting now...", 0x0D);
		//SKY_FATAL_ERROR(skyLogChannel::TXT_FILE, "skyDXWin32View: while creating the win32 window something went wrong! Couldn't register ClassEx. Aborting now...");

		return EResult_ERROR;
	}

	// once everything worked we can successfully say that 
	// we initialized the window!
	IsInitialized = true;

	//SKY_PRINT_CONSOLE("skyDXWin32View", "window class ex registered and created!", 0x0D);
	//SKY_INFO(skyLogChannel::TXT_FILE, "skyDXWin32View: window class ex registered and created!");

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

	this->HWnd = CreateWindowW ( ClassName.c_str(), Title.c_str() , WS_OVERLAPPEDWINDOW | WS_VISIBLE,
								CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, 
								NULL, NULL, Instance, NULL);

	// if anything went wrong we'd like to return an 
	// error thus we may be able to process it further
	if( !this->HWnd ) 
	{
		//SKY_FATAL_ERROR(skyLogChannel::TXT_FILE, "skyDXWin32View: couldn't create the actual window.");
		//SKY_PRINT_CONSOLE("skyDXWin32View", "couldn't create the actual window.", 0x0D);
		return EResult_ERROR;
	}

	ShowWindow ( this->HWnd, this->CmdShow );
	UpdateWindow ( this->HWnd ) ;
	IsCreated = true;

	//SKY_INFO(0, "skyDXWin32View: window shown and updated.");
	//SKY_PRINT_CONSOLE("skyDXWin32View", "window shown and updated.", 0x0D);

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
LRESULT CALLBACK CWindow::WndProc(HWND InHWnd, UINT InMsg, WPARAM InWParam, LPARAM InLParam)
{
	switch (InMsg)
	{
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
				return DefWindowProc(InHWnd, InMsg, InWParam, InLParam);
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