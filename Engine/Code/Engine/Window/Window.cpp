#include "Engine/Window/Window.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

//-----------------------------------------------------------------------------------------------
Window::Window( WindowConfig const& config )
	: m_config( config )
{

}

//-----------------------------------------------------------------------------------------------
Window::~Window()
{

}

//-----------------------------------------------------------------------------------------------
void Window::Startup()
{
	CreateOSWindow();
}

//-----------------------------------------------------------------------------------------------
void Window::Shutdown()
{

}

//-----------------------------------------------------------------------------------------------
void Window::BeginFrame()
{
	// Process OS messages (keyboard/mouse button clicked, application lost/gained focus, etc.)
	RunMessagePump(); // calls our own WindowsMessageHandlingProcedure() function for us!
}

//-----------------------------------------------------------------------------------------------
void Window::EndFrame()
{

}

//-----------------------------------------------------------------------------------------------
// Handles Windows (Win32) messages/events; i.e. the OS is trying to tell us something happened.
// This function is called back by Windows whenever we tell it to (by calling DispatchMessage).
//
// #SD1ToDo: Eventually, we will move this function to a more appropriate place (Engine/Window/Window.cpp) later on...
//
LRESULT CALLBACK WindowsMessageHandlingProcedure( HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam )
{
	switch ( wmMessageCode )
	{
		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
	case WM_CLOSE:
	{
		FireEvent("Quit");
		//return 0; // "Consumes" this message (tells Windows "okay, we handled it")
	}

	// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
	case WM_KEYDOWN:
	{
		unsigned char asKey = ( unsigned char )wParam;
		g_engine->m_input->HandleKeyPressed( asKey );
		break;
	}

	// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
	case WM_KEYUP:
	{
		unsigned char asKey = ( unsigned char )wParam;
		g_engine->m_input->HandleKeyReleased( asKey );
		break;
	}

	case WM_LBUTTONDOWN: // Like WM_KEYDOWN but for left mouse button
	{
		if( g_engine->m_input )
		{
			g_engine->m_input->HandleKeyPressed( KEYCODE_LEFT_MOUSE );
		}
		return 0; // "Consumes" this message (tells Windows "okay, we handled it")
	}

	case WM_LBUTTONUP: // Like WM_KEYUP but for left mouse button
	{
		if( g_engine->m_input )
		{
			g_engine->m_input->HandleKeyReleased( KEYCODE_LEFT_MOUSE );
		}
		return 0; // "Consumes" this message (tells Windows "okay, we handled it")
	}

	case WM_RBUTTONDOWN:
	{
		if ( g_engine->m_input )
		{
			g_engine->m_input->HandleKeyPressed( KEYCODE_RIGHT_MOUSE );
		}
		return 0;
	}

	case WM_RBUTTONUP:
	{
		if ( g_engine->m_input )
		{
			g_engine->m_input->HandleKeyReleased( KEYCODE_RIGHT_MOUSE );
		}
		return 0;
	}
	}

	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	return DefWindowProc( windowHandle, wmMessageCode, wParam, lParam );
}

//-----------------------------------------------------------------------------------------------
void Window::CreateOSWindow()
{
	HINSTANCE applicationInstanceHandle = ::GetModuleHandle( NULL );
	SetProcessDpiAwarenessContext( DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 );

	// Define a window style/class
	WNDCLASSEX windowClassDescription;
	memset( &windowClassDescription, 0, sizeof( windowClassDescription ) );
	windowClassDescription.cbSize = sizeof( windowClassDescription );
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast< WNDPROC >( WindowsMessageHandlingProcedure ); // Register our Windows message-handling function
	windowClassDescription.hInstance = GetModuleHandle( NULL );
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT( "Simple Window Class" );
	RegisterClassEx( &windowClassDescription );

	// #SD1ToDo: Add support for fullscreen mode (requires different window style flags than windowed mode)
	DWORD const windowStyleFlags = WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_OVERLAPPED;
	DWORD const windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect( desktopWindowHandle, &desktopRect );
	float desktopWidth = ( float )( desktopRect.right - desktopRect.left );
	float desktopHeight = ( float )( desktopRect.bottom - desktopRect.top );
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	constexpr float maxClientFractionOfDesktop = 0.90f;
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;
	if ( m_config.m_clientAspect > desktopAspect )
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / m_config.m_clientAspect;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		clientWidth = clientHeight * m_config.m_clientAspect;
	}

	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0.5f * ( desktopWidth - clientWidth );
	float clientMarginY = 0.5f * ( desktopHeight - clientHeight );
	RECT clientRect;
	clientRect.left = ( int )clientMarginX;
	clientRect.right = clientRect.left + ( int )clientWidth;
	clientRect.top = ( int )clientMarginY;
	clientRect.bottom = clientRect.top + ( int )clientHeight;

	m_clientDimensions = IntVec2( clientRect.right, clientRect.bottom );

	// Calculate the outer dimensions of the physical window, including frame et. al.
	RECT windowRect = clientRect;
	AdjustWindowRectEx( &windowRect, windowStyleFlags, FALSE, windowStyleExFlags );

	WCHAR windowTitle[1024];
	MultiByteToWideChar( GetACP(), 0, m_config.m_windowTitle.c_str(), -1, windowTitle, sizeof( windowTitle ) / sizeof( windowTitle[0] ) );

	m_windowHandle = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		( HINSTANCE )applicationInstanceHandle,
		NULL );

	/*ShowWindow( reinterpret_cast< HWND >( m_windowHandle ), SW_SHOW );
	SetForegroundWindow( reinterpret_cast< HWND >( m_windowHandle ) );
	SetFocus( reinterpret_cast< HWND >( m_windowHandle ) );*/

	//m_displayDeviceContext = GetDC( reinterpret_cast< HWND >( m_windowHandle ) );

	HWND hwnd = static_cast< HWND >( m_windowHandle );
	ShowWindow( hwnd, SW_SHOW );
	SetForegroundWindow( hwnd );
	SetFocus( hwnd );

	m_displayDeviceContext = GetDC( hwnd );

	HCURSOR cursor = LoadCursor( NULL, IDC_ARROW );
	SetCursor( cursor );
}

//-----------------------------------------------------------------------------------------------
// Processes all Windows messages (WM_xxx) for this app that have queued up since last frame.
// For each message in the queue, our WindowsMessageHandlingProcedure (or "WinProc") function
//	is called, telling us what happened (key up/down, minimized/restored, gained/lost focus, etc.)
//
// #SD1ToDo: Eventually, we will move this function to a more appropriate place later on... (Engine/Window/Window.cpp)
//
void Window::RunMessagePump()
{
	MSG queuedMessage;
	for ( ;; )
	{
		BOOL const wasMessagePresent = PeekMessage( &queuedMessage, NULL, 0, 0, PM_REMOVE );
		if ( !wasMessagePresent )
		{
			break;
		}

		TranslateMessage( &queuedMessage );
		DispatchMessage( &queuedMessage ); // This tells Windows to call our "WindowsMessageHandlingProcedure" (a.k.a. "WinProc") function
	}
}

//-----------------------------------------------------------------------------------------------
void* Window::GetHwnd() const
{
	return m_windowHandle;
}

//-----------------------------------------------------------------------------------------------
IntVec2 Window::GetClientDimensions() const
{
	return m_clientDimensions;
}

//-------------------------------------------------------------------------------------------
// Returns the mouse cursor's current position relative to the interior client area of our
// window, in normalized UV coordinates -- (0,0) is bottom-left, (1,1) is top-right.
//-------------------------------------------------------------------------------------------
Vec2 Window::GetNormalizedMouseUV() const
{
    HWND windowHandle = static_cast< HWND >( m_windowHandle ); // Need to add this new void* member!
    POINT cursorCoords;
    RECT clientRect;
    ::GetCursorPos( &cursorCoords ); // in Windows screen coordinates; (0,0) is top-left
    ::ScreenToClient( windowHandle, &cursorCoords ); // get cursor relative to this window's client area
    ::GetClientRect( windowHandle, &clientRect ); // dimensions of client area (0,0 to width,height)
    float cursorX = static_cast< float >( cursorCoords.x ) / static_cast< float >( clientRect.right );
    float cursorY = static_cast< float >( cursorCoords.y ) / static_cast< float >( clientRect.bottom );
    return Vec2( cursorX, 1.f - cursorY ); // Flip Y; we want (0,0) bottom-left, not top-left
}