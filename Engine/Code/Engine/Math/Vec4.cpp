#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/MathUtils.hpp"
#include<math.h>


//-----------------------------------------------------------------------------------------------
Vec4::Vec4( Vec4 const& copy )
	: x( copy.x )
	, y( copy.y )
	, z( copy.z )
	, w( copy.w )
{
}


//-----------------------------------------------------------------------------------------------
Vec4::Vec4( float initialX, float initialY, float initialZ, float initialW )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
	, w( initialW )
{
}


//-----------------------------------------------------------------------------------------------
Vec4 const Vec4::operator + ( Vec4 const& vecToAdd ) const
{
	return Vec4( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z, w + vecToAdd.w );
}


//-----------------------------------------------------------------------------------------------
Vec4 const Vec4::operator-( Vec4 const& vecToSubtract ) const
{
	return Vec4( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z, w - vecToSubtract.w );
}


//------------------------------------------------------------------------------------------------
Vec4 const Vec4::operator-() const
{
	return Vec4( -x, -y, -z, -w );
}

//-----------------------------------------------------------------------------------------------
Vec4 const Vec4::operator*( float uniformScale ) const
{
	return Vec4( x * uniformScale, y * uniformScale, z * uniformScale, w * uniformScale );
}


//------------------------------------------------------------------------------------------------
Vec4 const Vec4::operator*( Vec4 const& vecToMultiply ) const
{
	return Vec4( x * vecToMultiply.x, y * vecToMultiply.y, z * vecToMultiply.z, w * vecToMultiply.w );
}


//-----------------------------------------------------------------------------------------------
Vec4 const Vec4::operator/( float inverseScale ) const
{
	return Vec4( x / inverseScale, y / inverseScale, z / inverseScale, w / inverseScale );
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator+=( Vec4 const& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
	w += vecToAdd.w;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator-=( Vec4 const& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
	w -= vecToSubtract.w;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
	w *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
	w /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator=( Vec4 const& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
	w = copyFrom.z;
}


//-----------------------------------------------------------------------------------------------
Vec4 const operator*( float uniformScale, Vec4 const& vecToScale )
{
	return Vec4( vecToScale.x * uniformScale, vecToScale.y * uniformScale, vecToScale.z * uniformScale, vecToScale.w * uniformScale );
}


//-----------------------------------------------------------------------------------------------
bool Vec4::operator==( Vec4 const& compare ) const
{
	return x == compare.x && y == compare.y && z == compare.z && w == compare.w;
}


//-----------------------------------------------------------------------------------------------
bool Vec4::operator!=( Vec4 const& compare ) const
{
	return  x != compare.x || y != compare.y || z != compare.z || w != compare.w;
}

float Vec4::GetLength() const
{
	return sqrtf( GetLengthSquared() );
}

float Vec4::GetLengthXY() const
{
	return sqrtf( GetLengthXYSquared() );
}

float Vec4::GetLengthXYSquared() const
{
	return ( x * x ) + ( y * y );
}

float Vec4::GetLengthSquared() const
{
	return ( x * x ) + ( y * y ) + ( z * z ) + ( w * w );
}

float Vec4::GetOrientationAboutZDegrees() const
{
	return ConvertRadiansToDegrees( GetOrientationAboutZRadians() );
}

float Vec4::GetOrientationAboutZRadians() const
{
	return atan2f( y, x );
}

Vec4 const Vec4::GetRotatedAboutZDegrees( float rotationDegreesAboutZ ) const
{
	float cosTheta = CosDegrees( rotationDegreesAboutZ );
	float sinTheta = SinDegrees( rotationDegreesAboutZ );
	return Vec4( ( x * cosTheta ) - ( y * sinTheta ), ( x * sinTheta ) + ( y * cosTheta ), z, w );
}

Vec4 const Vec4::GetRotatedAboutZRadians( float rotationRadiansAboutZ ) const
{
	float cosTheta = cosf( rotationRadiansAboutZ );
	float sinTheta = sinf( rotationRadiansAboutZ );
	return Vec4( ( x * cosTheta ) - ( y * sinTheta ), ( x * sinTheta ) + ( y * cosTheta ), z, w );
}

Vec4 const Vec4::GetClampedToMaxLength( float maxLength ) const
{
	float length = GetLength();
	if ( length > maxLength )
	{
		float scale = maxLength / length;
		return Vec4( ( x * scale ), ( y * scale ), ( z * scale ), ( w * scale ) );
	}
	return Vec4( x, y, z, w );
}

Vec4 const Vec4::GetNormalized() const
{
	float length = GetLength();
	if ( length == 0 )
		return Vec4( 0, 0, 0, 0 );

	return Vec4( x / length, y / length, z / length, w / length );
}
