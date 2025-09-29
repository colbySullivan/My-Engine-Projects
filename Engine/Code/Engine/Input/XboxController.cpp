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
    return m_buttons[(int) buttonID].m_wasPressedLastFrame;
}

//------------------------------------------------------------------------------
bool XboxController::WasButtonJustReleased(XboxButtonID buttonID) const
{
    return (m_buttons[(int) buttonID].m_wasPressedLastFrame  && !m_buttons[(int) buttonID].m_isPressed);
}

//------------------------------------------------------------------------------
void XboxController::RefreshStatus()
{
    XINPUT_STATE xboxControllerState = {}; // Clear (zero-out) the controller state structure
    for (int controllerNumber = 0; controllerNumber < 3; ++controllerNumber)
    {
        DWORD errorCode = XInputGetState( controllerNumber, &xboxControllerState ); // Get fresh state info
        if( errorCode == ERROR_SUCCESS ){ // Result if the controller is connected (error code is SUCCESS)
            bool isButtonADown = (xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A) == XINPUT_GAMEPAD_A;
            bool isButtonBDown = (xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_B) == XINPUT_GAMEPAD_B;
            bool isButtonXDown = (xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_X) == XINPUT_GAMEPAD_X;
            bool isButtonYDown = (xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) == XINPUT_GAMEPAD_Y;
            bool isButtonBackDown = (xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) == XINPUT_GAMEPAD_BACK;
            bool isButtonStartDown = (xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_START) == XINPUT_GAMEPAD_START;
            bool isButtonLeftThumbDown = (xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) == XINPUT_GAMEPAD_LEFT_THUMB;
            bool isButtonRightThumbDown = (xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) == XINPUT_GAMEPAD_RIGHT_THUMB;
            bool isButtonLeftShoulderDown = (xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) == XINPUT_GAMEPAD_LEFT_SHOULDER;
            bool isButtonRightShoulderDown = (xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) == XINPUT_GAMEPAD_RIGHT_SHOULDER;
            bool isButtonDPadUpDown = (xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) == XINPUT_GAMEPAD_DPAD_UP;
            bool isButtonDPadDownDown = (xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) == XINPUT_GAMEPAD_DPAD_DOWN;
            bool isButtonDPadLeftDown = (xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) == XINPUT_GAMEPAD_DPAD_LEFT;
            bool isButtonDPadRightDown = (xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) == XINPUT_GAMEPAD_DPAD_RIGHT;
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
}

//------------------------------------------------------------------------------
void XboxController::Reset()
{

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

}