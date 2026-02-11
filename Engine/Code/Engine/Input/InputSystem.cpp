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