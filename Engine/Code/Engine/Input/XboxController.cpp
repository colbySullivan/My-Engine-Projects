#include "Engine/Input/XboxController.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // must #include Windows.h before #including Xinput.h
#include <Xinput.h> // include the Xinput API header file (interface)
#pragma comment( lib, "xinput" ) // Link in the xinput.lib static library

constexpr short XBOX_STICK_MAX = 32767;
constexpr short XBOX_STICK_MIN = -32768;
constexpr unsigned char XBOX_TRIGGER_MAX = 255;

constexpr float LEFT_STICK_INNER_DEADZONE = 0.3f;
constexpr float LEFT_STICK_OUTER_DEADZONE = 0.9f;
constexpr float RIGHT_STICK_INNER_DEADZONE = 0.3f;
constexpr float RIGHT_STICK_OUTER_DEADZONE = 0.9f;

//------------------------------------------------------------------------------
XboxController::XboxController( int controllerID )
    : m_id ( controllerID )
{
    m_leftStick.SetDeadZoneThresholds(LEFT_STICK_INNER_DEADZONE, LEFT_STICK_OUTER_DEADZONE);
    m_rightStick.SetDeadZoneThresholds(RIGHT_STICK_INNER_DEADZONE, RIGHT_STICK_OUTER_DEADZONE);
}

//------------------------------------------------------------------------------
XboxController::~XboxController()
{
}

//------------------------------------------------------------------------------
bool XboxController::IsConnected() const
{
    return m_isConnected;
}

//------------------------------------------------------------------------------
int XboxController::GetControllerID() const
{
    return m_id;
}

//------------------------------------------------------------------------------
AnalogJoystick const& XboxController::GetLeftStick() const
{
    return m_leftStick;
}

//------------------------------------------------------------------------------
AnalogJoystick const& XboxController::GetRightStick() const
{
    return m_rightStick;
}

//------------------------------------------------------------------------------
float XboxController::GetLeftTrigger() const
{
    return m_leftTrigger;
}

//------------------------------------------------------------------------------
float XboxController::GetRightTrigger() const
{
    return m_rightTrigger;
}

//------------------------------------------------------------------------------
KeyButtonState const& XboxController::GetButton(XboxButtonID buttonID) const
{
    return m_buttons[(int) buttonID];
}

//------------------------------------------------------------------------------
bool XboxController::IsButtonDown(XboxButtonID buttonID) const
{
    return m_buttons[(int) buttonID].m_isPressed;
}

//------------------------------------------------------------------------------
bool XboxController::WasButtonJustPressed(XboxButtonID buttonID) const
{
    return (!m_buttons[(int) buttonID].m_wasPressedLastFrame) && (m_buttons[(int) buttonID].m_isPressed);
}

//------------------------------------------------------------------------------
bool XboxController::WasButtonJustReleased(XboxButtonID buttonID) const
{
    return (m_buttons[(int) buttonID].m_wasPressedLastFrame  && !m_buttons[(int) buttonID].m_isPressed);
}

//------------------------------------------------------------------------------
void XboxController::RefreshStatus()
{
	XINPUT_STATE xboxControllerState = {};
	DWORD errorCode = XInputGetState(m_id, &xboxControllerState);

	if (errorCode == ERROR_SUCCESS)
	{
		m_isConnected = true;

		unsigned short buttons = xboxControllerState.Gamepad.wButtons;

		UpdateButton(XboxButtonID::A, buttons, XINPUT_GAMEPAD_A);                      // 0
		UpdateButton(XboxButtonID::B, buttons, XINPUT_GAMEPAD_B);                      // 1
		UpdateButton(XboxButtonID::X, buttons, XINPUT_GAMEPAD_X);                      // 2
		UpdateButton(XboxButtonID::Y, buttons, XINPUT_GAMEPAD_Y);                      // 3
		UpdateButton(XboxButtonID::LEFT_BUMPER, buttons, XINPUT_GAMEPAD_LEFT_SHOULDER);  
		UpdateButton(XboxButtonID::RIGHT_BUMPER, buttons, XINPUT_GAMEPAD_RIGHT_SHOULDER);
		UpdateButton(XboxButtonID::BACK, buttons, XINPUT_GAMEPAD_BACK);
		UpdateButton(XboxButtonID::START, buttons, XINPUT_GAMEPAD_START);
		UpdateButton(XboxButtonID::LEFT_STICK, buttons, XINPUT_GAMEPAD_LEFT_THUMB);
		UpdateButton(XboxButtonID::RIGHT_STICK, buttons, XINPUT_GAMEPAD_RIGHT_THUMB);
		UpdateButton(XboxButtonID::DPAD_UP, buttons, XINPUT_GAMEPAD_DPAD_UP);
		UpdateButton(XboxButtonID::DPAD_DOWN, buttons, XINPUT_GAMEPAD_DPAD_DOWN);
		UpdateButton(XboxButtonID::DPAD_LEFT, buttons, XINPUT_GAMEPAD_DPAD_LEFT);
		UpdateButton(XboxButtonID::DPAD_RIGHT, buttons, XINPUT_GAMEPAD_DPAD_RIGHT);

		UpdateJoystick(m_leftStick, xboxControllerState.Gamepad.sThumbLX, xboxControllerState.Gamepad.sThumbLY);
		UpdateJoystick(m_rightStick, xboxControllerState.Gamepad.sThumbRX, xboxControllerState.Gamepad.sThumbRY);
		UpdateTrigger(m_leftTrigger, xboxControllerState.Gamepad.bLeftTrigger);
		UpdateTrigger(m_rightTrigger, xboxControllerState.Gamepad.bRightTrigger);
	}
	else
	{
		if (m_isConnected)
		{
			Reset();
		}
		m_isConnected = false;
	}


}

//------------------------------------------------------------------------------
void XboxController::Reset()
{
	m_isConnected = false;
	m_leftTrigger = 0.f;
	m_rightTrigger = 0.f;

	for (int button = 0; button < (int)XboxButtonID::NUM; button++)
	{
		m_buttons[button].m_isPressed = false;
		m_buttons[button].m_wasPressedLastFrame = false;
	}

	m_leftStick.Reset();
	m_rightStick.Reset();
}

//------------------------------------------------------------------------------
void XboxController::UpdateJoystick(AnalogJoystick& out_joystick, short rawX, short rawY)
{

}

//------------------------------------------------------------------------------
void XboxController::UpdateTrigger(float& out_triggerValue, unsigned char rawValue)
{

}

//------------------------------------------------------------------------------
void XboxController::UpdateButton(XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag)
{
	KeyButtonState& button = m_buttons[(int)buttonID];
	button.m_wasPressedLastFrame = button.m_isPressed;
	button.m_isPressed = (buttonFlags & buttonFlag) == buttonFlag;
}