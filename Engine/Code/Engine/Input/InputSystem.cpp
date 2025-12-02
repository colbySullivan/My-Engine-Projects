#include "Engine/Input/InputSystem.hpp"
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
unsigned char const KEYCODE_DOWNARROW = 0x28;
unsigned char const KEYCODE_UPARROW = 0x26;
unsigned char const KEYCODE_LEFTARROW = 0x25;
unsigned char const KEYCODE_RIGHTARROW = 0x27;
unsigned char const KEYCODE_LEFT_MOUSE = VK_LBUTTON;
unsigned char const KEYCODE_RIGHT_MOUSE = VK_RBUTTON;

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
