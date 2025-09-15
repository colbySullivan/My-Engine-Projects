#include "Engine/Math/Vec3.hpp"
#include<math.h>
#include "MathUtils.hpp"
//#include "Engine/Math/MathUtils.hpp"
//#include "Engine/Core/EngineCommon.hpp"


//-----------------------------------------------------------------------------------------------
Vec3::Vec3( Vec3 const& copy )
	: x( copy.x )
	, y( copy.y )
	, z(copy.z)
{
}


//-----------------------------------------------------------------------------------------------
Vec3::Vec3( float initialX, float initialY, float initialZ)
	: x(initialX)
	, y(initialY)
	, z(initialZ)
{
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::operator + ( Vec3 const& vecToAdd ) const
{
	return Vec3( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z);
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::operator-( Vec3 const& vecToSubtract ) const
{
	return Vec3(x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z);
}


//------------------------------------------------------------------------------------------------
Vec3 const Vec3::operator-() const
{
	return Vec3( -x , -y , -z);
}

//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::operator*( float uniformScale ) const
{
	return Vec3( x * uniformScale, y * uniformScale, z * uniformScale);
}


//------------------------------------------------------------------------------------------------
Vec3 const Vec3::operator*( Vec3 const& vecToMultiply ) const
{
	return Vec3( x * vecToMultiply.x, y * vecToMultiply.y, z * vecToMultiply.z);
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::operator/( float inverseScale ) const
{
	return Vec3( x / inverseScale, y / inverseScale, z / inverseScale);
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator+=( Vec3 const& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator-=( Vec3 const& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator=( Vec3 const& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}


//-----------------------------------------------------------------------------------------------
Vec3 const operator*( float uniformScale, Vec3 const& vecToScale )
{
	return Vec3(vecToScale.x * uniformScale, vecToScale.y * uniformScale, vecToScale.z * uniformScale);
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator==( Vec3 const& compare ) const
{
	return x == compare.x && y == compare.y && z == compare.z;
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator!=( Vec3 const& compare ) const
{
	return  x != compare.x || y != compare.y || z != compare.z ;
}

float Vec3::GetLength() const
{
	return sqrtf(GetLengthSquared());
}

float Vec3::GetLengthXY() const
{
	return sqrtf(GetLengthXYSquared());
}

float Vec3::GetLengthXYSquared() const
{
	return (x * x) + (y * y);
}

float Vec3::GetLengthSquared() const
{
	return (x * x) + (y * y) + (z * z);
}

float Vec3::GetOrientationAboutZDegrees() const
{
	return ConvertRadiansToDegrees(GetOrientationAboutZRadians());
}

float Vec3::GetOrientationAboutZRadians() const
{
	return atan2f(y, x);
}

Vec3 const Vec3::GetRotatedAboutZDegrees(float rotationDegreesAboutZ) const
{
	float cosTheta = CosDegrees(rotationDegreesAboutZ);
	float sinTheta = SinDegrees(rotationDegreesAboutZ);
	return Vec3((x*cosTheta)-(y*sinTheta),(x*sinTheta)+(y*cosTheta),z);
}

Vec3 const Vec3::GetRotatedAboutZRadians(float rotationRadiansAboutZ) const
{
	float cosTheta = cosf(rotationRadiansAboutZ);
	float sinTheta = sinf(rotationRadiansAboutZ);
	return Vec3((x * cosTheta) - (y * sinTheta), (x * sinTheta) + (y * cosTheta), z);
}

Vec3 const Vec3::GetClampedToMaxLength(float maxLength) const
{
	float length = GetLength();
	if (length > maxLength)
	{
		float scale = maxLength / length;
		return Vec3((x * scale), (y * scale), (z * scale));
	}
	return Vec3(x, y, z);
}

Vec3 const Vec3::GetNormalized() const
{
	float length = GetLength();
	if (length == 0)
		return Vec3(0, 0, 0);
	return Vec3(x / length, y / length, z / length);
}
