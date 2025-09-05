#include "Engine/Math/Vec2.hpp"
#include "MathUtils.hpp"
#include<math.h>

//#include "Engine/Math/MathUtils.hpp"
//#include "Engine/Core/EngineCommon.hpp"


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( Vec2 const& copy )
	: x( copy.x )
	, y( copy.y )
{
}


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( float initialX, float initialY )
	: x(initialX)
	, y(initialY)
{
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::operator + ( Vec2 const& vecToAdd ) const
{
	return Vec2( x + vecToAdd.x, y + vecToAdd.y);
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::operator-( Vec2 const& vecToSubtract ) const
{
	return Vec2(x - vecToSubtract.x, y - vecToSubtract.y);
}


//------------------------------------------------------------------------------------------------
Vec2 const Vec2::operator-() const
{
	return Vec2( -x , -y );
}

// ToDo one of these multiplies is incorrect
//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::operator*( float uniformScale ) const
{
	return Vec2( x * uniformScale, y * uniformScale );
}


//------------------------------------------------------------------------------------------------
Vec2 const Vec2::operator*( Vec2 const& vecToMultiply ) const
{
	return Vec2( x * vecToMultiply.x, y * vecToMultiply.y );
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::operator/( float inverseScale ) const
{
	return Vec2( x / inverseScale, y / inverseScale );
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator+=( Vec2 const& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator-=( Vec2 const& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator=( Vec2 const& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}


//-----------------------------------------------------------------------------------------------
Vec2 const operator*( float uniformScale, Vec2 const& vecToScale )
{
	return Vec2(vecToScale.x * uniformScale, vecToScale.y * uniformScale);
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator==( Vec2 const& compare ) const
{
	return x == compare.x && y == compare.y;
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator!=( Vec2 const& compare ) const
{
	return  x != compare.x || y != compare.y;
}

//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::MakeFromPolarDegrees(float orientationDegrees, float length)
{
	float radians = ConvertDegreesToRadians(orientationDegrees);
	return Vec2(length * CosDegrees(radians), length * SinDegrees(radians));
}

//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::MakeFromPolarRadians(float orientationRadians, float length)
{
	return Vec2(length * CosDegrees(orientationRadians), length * SinDegrees(orientationRadians));
}

//-----------------------------------------------------------------------------------------------
float Vec2::GetLength() const
{
	return sqrtf(GetLengthSquared());
}

//-----------------------------------------------------------------------------------------------
float Vec2::GetLengthSquared() const
{
	return (x * x) + (y * y);
}

//-----------------------------------------------------------------------------------------------
float Vec2::GetOrientationDegrees() const
{
	return ConvertRadiansToDegrees(GetOrientationRadians());
}

//-----------------------------------------------------------------------------------------------
float Vec2::GetOrientationRadians() const
{
	return Atan2Degrees(y, x);
}

//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetRotatedBy90Degrees() const
{
	return Vec2( -y, x );
}

//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetRotatedByMinus90Degrees() const
{
	return Vec2( y, -x );
}

//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetRotatedByDegrees(float rotationDegrees)
{
	float radians = ConvertDegreesToRadians(rotationDegrees);
	float theta = Atan2Degrees(y, x);
	float R = GetLength();
	theta += radians;
	return Vec2(R * CosDegrees(theta), R * SinDegrees(theta));
}

//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetRotatedByRadians(float rotationRadians)
{
	float theta = Atan2Degrees(y, x);
	float R = GetLength();
	theta += rotationRadians;
	return Vec2(R * CosDegrees(theta), R * SinDegrees(theta));
}

//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetClamped(float maxLength) const
{
	float length = GetLength();
	if (length > maxLength)
	{
		float scale = maxLength / length;
		return Vec2(( x * scale ), ( y * scale ));
	}
	return Vec2(x,y);
}

//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetNormalized() const
{
	float length = GetLength();
	if (length == 0)
		return Vec2(0, 0);
	return Vec2( x / length, y / length);
}

// Try to incorporate GetRotatedByDegrees()
//-----------------------------------------------------------------------------------------------
void Vec2::SetOrientationDegrees(float newOrientationDegrees)
{
	float length = GetLength();
	float radians = ConvertDegreesToRadians(newOrientationDegrees);
	x, y = length * CosDegrees(radians), length* SinDegrees(radians);
}

//-----------------------------------------------------------------------------------------------
void Vec2::SetOrientationRadians(float newOrientationRadians)
{
	float length = GetLength();
	x, y = length * CosDegrees(newOrientationRadians), length* SinDegrees(newOrientationRadians);
}

//-----------------------------------------------------------------------------------------------
void Vec2::SetPolarDegrees(float newOrientationDegrees, float newLength)
{
	Vec2 degreesChangedVec = MakeFromPolarDegrees( newOrientationDegrees, newLength);
	x, y = degreesChangedVec.x, degreesChangedVec.y;
}

//-----------------------------------------------------------------------------------------------
void Vec2::SetPolarRadians(float newOrientationRadians, float newLength)
{
	Vec2 radiansChangedVec = MakeFromPolarRadians(newOrientationRadians, newLength);
	x, y = (radiansChangedVec.x, radiansChangedVec.y);
}

//-----------------------------------------------------------------------------------------------
void Vec2::Normalize()
{
	float length = GetLength();
	float scale = 1.f / length;
	x, y = (scale * x, scale * y);
}

//-----------------------------------------------------------------------------------------------
float Vec2::NormalizeAndGetPreviousLength()
{
	float length = GetLength();
	Normalize();
	return length;
}

//-----------------------------------------------------------------------------------------------
void Vec2::Rotate90Degrees()
{
	float buffer = x;
	x = -y;
	y = buffer;
}

//-----------------------------------------------------------------------------------------------
void Vec2::RotateMinus90Degrees()
{
	float buffer = x;
	x = y;
	y = -buffer;
}

//-----------------------------------------------------------------------------------------------
void Vec2::RotateDegrees(float rotationDegrees)
{
	Vec2 degreeRotatedVec = GetRotatedByDegrees(rotationDegrees);
	x, y = degreeRotatedVec.x, degreeRotatedVec.y;
}

//-----------------------------------------------------------------------------------------------
void Vec2::RotateRadians(float rotationRadians)
{
	Vec2 degreeRotatedVec = GetRotatedByRadians(rotationRadians);
	x, y = degreeRotatedVec.x, degreeRotatedVec.y;
}

//-----------------------------------------------------------------------------------------------
void Vec2::SetLength(float newLength)
{
	float length = GetLength();
	if (length == 0)
	{
		x = newLength;
		y = 0;
	}
	float scale = newLength / length;
	x *= scale;
	y *= scale;
}

//-----------------------------------------------------------------------------------------------
void Vec2::ClampLength(float maxLength)
{
	Vec2 clampedVec = GetClamped(maxLength);
	x, y = clampedVec.x, clampedVec.y;
}
