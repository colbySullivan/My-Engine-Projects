#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Engine.hpp"
#include <Windows.h> 
#include <stdio.h>

unsigned char const KEYCODE_F1 = 0x70;
unsigned char const KEYCODE_F2 = 0x71;
unsigned char const KEYCODE_F3 = 0x72;
unsigned char const KEYCODE_F4 = 0x73;
unsigned char const KEYCODE_F5 = 0x74;
unsigned char const KEYCODE_F6 = 0x75;
unsigned char const KEYCODE_F7 = 0x76;
unsigned char const KEYCODE_F8 = 0x77;
unsigned char const KEYCODE_F9 = 0x78;
unsigned char const KEYCODE_F10 = 0x79;
unsigned char const KEYCODE_F11 = 0x7A;
unsigned char const KEYCODE_F12 = 0x7B;
unsigned char const KEYCODE_ESC = 0x1B;
unsigned char const KEYCODE_TILDA = 0xC0;
unsigned char const KEYCODE_DOWNARROW = 0x28;
unsigned char const KEYCODE_UPARROW = 0x26;
unsigned char const KEYCODE_LEFTARROW = 0x25;
unsigned char const KEYCODE_RIGHTARROW = 0x27;
unsigned char const KEYCODE_LEFT_MOUSE = VK_LBUTTON;
unsigned char const KEYCODE_RIGHT_MOUSE = VK_RBUTTON;

// Dev console
unsigned char const KEYCODE_TILDE = 0xC0;
//unsigned char const KEYCODE_UPARROW = VK_UP;
//unsigned char const KEYCODE_DOWNARROW = VK_DOWN;
//unsigned char const KEYCODE_LEFTARROW = VK_LEFT;
//unsigned char const KEYCODE_RIGHTARROW = VK_RIGHT;
unsigned char const KEYCODE_ENTER = VK_RETURN;
unsigned char const KEYCODE_BACKSPACE = VK_BACK;
unsigned char const KEYCODE_INSERT = VK_INSERT;
unsigned char const KEYCODE_DELETE = VK_DELETE;
unsigned char const KEYCODE_HOME = VK_HOME;
unsigned char const KEYCODE_END = VK_END;
unsigned char const KEYCODE_SHIFT = VK_SHIFT;

InputSystem::InputSystem( InputConfig const& config ) 
	: m_config( config )
{
}

//-----------------------------------------------------------------------------------------------
InputSystem::~InputSystem()
{
	
}

//-----------------------------------------------------------------------------------------------
void InputSystem::Startup()
{
	g_engine->m_eventSystem->SubscribeEventCallbackFunction("KeyPressed", InputSystem::Event_KeyPressed);
	g_engine->m_eventSystem->SubscribeEventCallbackFunction("KeyReleased", InputSystem::Event_KeyReleased);
	for (int i = 0; i < NUM_KEYCODES; ++i)
	{
		m_keyStates[i].m_isPressed = false;
		m_keyStates[i].m_wasPressedLastFrame = false;
	}
}

//-----------------------------------------------------------------------------------------------
void InputSystem::Shutdown()
{

}

//-----------------------------------------------------------------------------------------------
void InputSystem::BeginFrame()
{
	for ( int controllerID = 0; controllerID < NUM_XBOX_CONTROLLERS; ++controllerID )
	{
		m_controllers[controllerID].RefreshStatus();
	}

	CheckCursorVisibility();

	HWND hwnd = ( HWND )g_engine->m_window->GetHwnd();

	m_prevCursorClientPos = m_cursorState.m_cursorClientPosition;

	POINT cursorScreen;
	GetCursorPos( &cursorScreen );
	POINT cursorClient = cursorScreen;
	ScreenToClient( hwnd, &cursorClient );
	Vec2 curClientPos = Vec2( ( float )cursorClient.x, ( float )cursorClient.y );


	if ( m_cursorState.m_cursorMode == CursorMode::FPS )
	{
		m_cursorState.m_cursorClientDelta.x = curClientPos.x - m_prevCursorClientPos.x;
		m_cursorState.m_cursorClientDelta.y = curClientPos.y - m_prevCursorClientPos.y;

		RECT clientRect;
		GetClientRect( hwnd, &clientRect );
		POINT center = { ( clientRect.right - clientRect.left ) / 2, ( clientRect.bottom - clientRect.top ) / 2 };
		ClientToScreen( hwnd, &center );

		SetCursorPos( center.x, center.y );

		POINT anchorScreen;
		if ( GetCursorPos( &anchorScreen ) )
		{
			POINT anchorClient = anchorScreen;
			ScreenToClient( hwnd, &anchorClient );
			m_cursorState.m_cursorClientPosition = Vec2( ( float )anchorClient.x, ( float )anchorClient.y );
		}
	}
	else
	{
		m_cursorState.m_cursorClientDelta = Vec2( 0.f, 0.f );
		m_cursorState.m_cursorClientPosition = curClientPos;
	}
}

//-----------------------------------------------------------------------------------------------
void InputSystem::EndFrame()
{
	for (int i = 0; i < NUM_KEYCODES; ++i)
	{
		m_keyStates[i].m_wasPressedLastFrame = m_keyStates[i].m_isPressed;
	}
}

//-----------------------------------------------------------------------------------------------
bool InputSystem::WasKeyJustPressed(unsigned char keyCode)
{
	return m_keyStates[keyCode].m_isPressed && !m_keyStates[keyCode].m_wasPressedLastFrame;
}

//-----------------------------------------------------------------------------------------------
bool InputSystem::WasKeyJustReleased(unsigned char keyCode)
{
	return !m_keyStates[keyCode].m_isPressed && m_keyStates[keyCode].m_wasPressedLastFrame;
}

//-----------------------------------------------------------------------------------------------
bool InputSystem::IsKeyDown(unsigned char keyCode)
{
	return m_keyStates[keyCode].m_isPressed;
}

//-----------------------------------------------------------------------------------------------
void InputSystem::HandleKeyPressed(unsigned char keyCode)
{
	m_keyStates[keyCode].m_isPressed = true;
}

//-----------------------------------------------------------------------------------------------
void InputSystem::HandleKeyReleased(unsigned char keyCode)
{
	m_keyStates[keyCode].m_isPressed = false;
}

//-----------------------------------------------------------------------------------------------
XboxController const& InputSystem::GetController(int controllerID) const
{
	return m_controllers[controllerID];
}

//-----------------------------------------------------------------------------------------------
void InputSystem::SetCursorMode( CursorMode cursorMode )
{
	m_cursorState.m_cursorMode = cursorMode;
}

//-----------------------------------------------------------------------------------------------
Vec2 InputSystem::GetCursorClientDelta() const
{
	return m_cursorState.m_cursorClientDelta;
}

//-----------------------------------------------------------------------------------------------
Vec2 InputSystem::GetCursorClientPosition() const
{
	return m_cursorState.m_cursorClientPosition;
}

//-----------------------------------------------------------------------------------------------
Vec2 InputSystem::GetCursorNormalizedPosition() const
{
	HWND hwnd = ( HWND )g_engine->m_window->GetHwnd();
	RECT clientRect;
	if ( GetClientRect( hwnd, &clientRect ) )
	{
		float width = ( float )( clientRect.right - clientRect.left );
		float height = ( float )( clientRect.bottom - clientRect.top );

		if ( width > 0.f && height > 0.f )
		{
			return Vec2( m_cursorState.m_cursorClientPosition.x / width, m_cursorState.m_cursorClientPosition.y / height );
		}
	}
	return Vec2( 0.f, 0.f );
}

//-----------------------------------------------------------------------------------------------
bool InputSystem::Event_KeyPressed( EventArgs& args )
{
	if ( !g_engine->m_input )
	{
		return false;
	}
	unsigned char keyCode = ( unsigned char )args.GetValue( "KeyCode", -1 );
	g_engine->m_input->HandleKeyPressed( keyCode );
	return true;
}

//-----------------------------------------------------------------------------------------------
bool InputSystem::Event_KeyReleased( EventArgs& args )
{
	if ( !g_engine->m_input )
	{
		return false;
	}
	unsigned char keyCode = ( unsigned char )args.GetValue( "KeyCode", -1 );
	g_engine->m_input->HandleKeyReleased( keyCode );
	return true;
}

void InputSystem::CheckCursorVisibility()
{
	// https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-cursorinfo
	CURSORINFO ci = { sizeof( CURSORINFO ) };
	GetCursorInfo( &ci );
	bool windowsCursorVisible = ( ci.flags & CURSOR_SHOWING ) != 0;

	bool wantVisible = (m_cursorState.m_cursorMode == CursorMode::POINTER);

	if ( windowsCursorVisible == wantVisible )
		return;

	if ( wantVisible )
	{
		int count = ::ShowCursor( TRUE );
		while ( count < 0 )
		{
			count = ::ShowCursor( TRUE );
		}
	}
	else
	{

		int count = ::ShowCursor( FALSE );
		while ( count >= 0 )
		{
			count = ::ShowCursor( FALSE );
		} 
	}
}