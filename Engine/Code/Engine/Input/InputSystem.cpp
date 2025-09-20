#include "Engine/Input/InputSystem.hpp"
#include <Windows.h> 
#include <stdio.h>

InputSystem::InputSystem()
{
	m_input = this;
}

InputSystem::~InputSystem()
{
	
}

void InputSystem::Startup()
{
	for (int i = 0; i < 256; ++i)
	{
		m_isKeyDownArray[i] = false;
		m_wasKeyDownPrevArray[i] = false;
	}
}

void InputSystem::Shutdown()
{

}

void InputSystem::BeginFrame()
{
	for (int i = 0; i < 256; ++i)
	{
		m_wasKeyDownPrevArray[i] = m_isKeyDownArray[i];
	}
}

void InputSystem::EndFrame()
{

}

bool InputSystem::WasKeyJustPressed(unsigned char keyCode)
{
	return m_isKeyDownArray[keyCode] &&  !m_wasKeyDownPrevArray[keyCode];
}

bool InputSystem::WasKeyJustReleased(unsigned char keyCode)
{
	return !m_isKeyDownArray[keyCode] && m_wasKeyDownPrevArray[keyCode];
}

bool InputSystem::IsKeyDown(unsigned char keyCode)
{
	return m_isKeyDownArray[keyCode];
}

void InputSystem::HandleKeyPressed(unsigned char keyCode)
{
	m_isKeyDownArray[ keyCode ] = true;
}

void InputSystem::HandleKeyReleased(unsigned char keyCode)
{
	m_isKeyDownArray[ keyCode ] = false;
}