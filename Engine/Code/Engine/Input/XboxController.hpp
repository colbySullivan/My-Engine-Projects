#pragma once

#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Input/KeyButtonState.hpp"

enum class XboxButtonID {
	A = 0,
	B = 1,
	X = 2,
	Y = 3,
	LEFT_BUMPER = 4,
	RIGHT_BUMPER = 5,
	LEFT_TRIGGER = 6,
	RIGHT_TRIGGER = 7,
	BACK = 8,
	START = 9,
	LEFT_STICK = 10,
	RIGHT_STICK = 11,
	DPAD_UP = 12,
	DPAD_DOWN = 13,
	DPAD_LEFT = 14,
	DPAD_RIGHT = 15,

    NUM = 16
};
//------------------------------------------------------------------------------

constexpr short XBOX_STICK_MAX = 32767;
constexpr short XBOX_STICK_MIN = -32768;
constexpr unsigned char XBOX_TRIGGER_MAX = 255;

//------------------------------------------------------------------------------
class XboxController
{
    friend class InputSystem;

public:
    bool                IsConnected() const;
    int                 GetControllerID() const;
    AnalogJoystick const& GetLeftStick() const;
    AnalogJoystick const& GetRightStick() const;
    float               GetLeftTrigger() const;
    float               GetRightTrigger() const;
    KeyButtonState const& GetButton( XboxButtonID buttonID ) const;
    bool                IsButtonDown( XboxButtonID buttonID ) const;
    bool                WasButtonJustPressed( XboxButtonID buttonID ) const;
    bool                WasButtonJustReleased( XboxButtonID buttonID ) const;

private:
    XboxController( int controllerID);
    ~XboxController();
    void RefreshStatus();
    void Reset();
    void UpdateJoystick( AnalogJoystick& out_joystick, short rawX, short rawY );
    void UpdateTrigger( float& out_triggerValue, unsigned char rawValue );
    void UpdateButton( XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag );

private:
    int                 m_id                = -1;
    bool                m_isConnected       = false;
    float               m_leftTrigger       = 0.f;
    float               m_rightTrigger      = 0.f;
    KeyButtonState      m_buttons[ (int) XboxButtonID::NUM ];
    AnalogJoystick      m_leftStick;
    AnalogJoystick      m_rightStick;
};