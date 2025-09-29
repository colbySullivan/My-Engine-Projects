#include "AnalogJoystick.hpp"
#include "Engine/Math/MathUtils.hpp"


//------------------------------------------------------------------------------
Vec2 AnalogJoystick::GetPosition() const
{
	return Vec2(0,0); // TODO
}

//------------------------------------------------------------------------------
float AnalogJoystick::GetMagnitude() const
{
	return 0.0f; // TODO
}

//------------------------------------------------------------------------------
float AnalogJoystick::GetOrientationDegrees() const
{
	return 0.0f; // TODO
}

//------------------------------------------------------------------------------
Vec2 AnalogJoystick::GetRawUncorrectedPosition() const
{
	return Vec2(0,0); // TODO
}

//------------------------------------------------------------------------------
float AnalogJoystick::GetInnerDeadZoneFraction() const
{
 return 0.0f; // TODO
}

//------------------------------------------------------------------------------
float AnalogJoystick::GetOuterDeadZoneFraction() const
{
	return 0.0f; // TODO
}

//------------------------------------------------------------------------------
void AnalogJoystick::Reset()
{
	
}

//------------------------------------------------------------------------------
void AnalogJoystick::SetDeadZoneThresholds(float normalizedInnerDeadzoneThreshold, float normalizedOuterDeadzoneThreshold)
{

}

//------------------------------------------------------------------------------

void AnalogJoystick::UpdatePosition(float rawNormalizedX, float rawNormalizedY)
{

}