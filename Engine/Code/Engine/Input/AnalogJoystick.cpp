#include "AnalogJoystick.hpp"
#include "Engine/Math/MathUtils.hpp"


//------------------------------------------------------------------------------
Vec2 AnalogJoystick::GetPosition() const
{
	return m_correctedPosition;
}

//------------------------------------------------------------------------------
float AnalogJoystick::GetMagnitude() const
{
	return m_correctedPosition.GetLength();
}

//------------------------------------------------------------------------------
float AnalogJoystick::GetOrientationDegrees() const
{
	return m_correctedPosition.GetOrientationDegrees();
}

//------------------------------------------------------------------------------
Vec2 AnalogJoystick::GetRawUncorrectedPosition() const
{
	return m_rawPosition;
}

//------------------------------------------------------------------------------
float AnalogJoystick::GetInnerDeadZoneFraction() const
{
	return m_innerDeadZoneFraction;
}

//------------------------------------------------------------------------------
float AnalogJoystick::GetOuterDeadZoneFraction() const
{
	return m_outerDeadZoneFraction;
}

//------------------------------------------------------------------------------
void AnalogJoystick::Reset()
{
	m_rawPosition = Vec2(0.0f, 0.0f);
	m_correctedPosition = Vec2(0.0f, 0.0f);
}

//------------------------------------------------------------------------------
void AnalogJoystick::SetDeadZoneThresholds(float normalizedInnerDeadzoneThreshold, float normalizedOuterDeadzoneThreshold)
{
	m_innerDeadZoneFraction = normalizedInnerDeadzoneThreshold;
	m_outerDeadZoneFraction = normalizedOuterDeadzoneThreshold;
}

//------------------------------------------------------------------------------

void AnalogJoystick::UpdatePosition(float rawNormalizedX, float rawNormalizedY)
{
	// Normalized
	m_rawPosition = Vec2(rawNormalizedX, rawNormalizedY);

	// PolarUncorrected
	float r = m_rawPosition.GetLength();
	float theta = m_rawPosition.GetOrientationRadians();

	// PolarCorrected
	if (r < m_innerDeadZoneFraction)
	{
		m_correctedPosition = Vec2(0.0f, 0.0f);
	}
	else
	{
		if (r > m_outerDeadZoneFraction)
			r = 1.0f;
		else
			r = RangeMapClamped(r, m_innerDeadZoneFraction, m_outerDeadZoneFraction, 0.0f, 1.0f);

		// XYCorrected
		m_correctedPosition = Vec2::MakeFromPolarRadians(theta, r);
	}
}