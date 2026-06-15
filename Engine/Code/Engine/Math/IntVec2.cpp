#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include<math.h>


IntVec2::IntVec2(IntVec2 const& copyFrom)
	: x(copyFrom.x)
	, y(copyFrom.y)
{

}

//-----------------------------------------------------------------------------------------------
IntVec2::IntVec2(int initialX, int initialY)
	: x(initialX)
	, y(initialY)
{

}

//-----------------------------------------------------------------------------------------------
float IntVec2::GetLength() const
{
	return static_cast<float>(sqrt((x * x) + (y * y)));
}

//-----------------------------------------------------------------------------------------------
int IntVec2::GetTaxicabLength() const
{
	return abs(x) + abs(y);
}

//-----------------------------------------------------------------------------------------------
int IntVec2::GetLengthSquared() const
{
	return (x * x) + (y * y);
}

//-----------------------------------------------------------------------------------------------
float IntVec2::GetOrientationRadians() const
{
	return static_cast<float>(atan2(y, x));
}

//-----------------------------------------------------------------------------------------------
float IntVec2::GetOrientationDegrees() const
{
	return ConvertRadiansToDegrees(static_cast<float>(atan2(y, x)));
}

//-----------------------------------------------------------------------------------------------
IntVec2 const IntVec2::GetRotatedBy90Degrees() const
{
	return IntVec2( -y, x );
}

//-----------------------------------------------------------------------------------------------
IntVec2 const IntVec2::GetRotatedByMinus90Degrees() const
{
	return IntVec2( y, -x );
}

//-----------------------------------------------------------------------------------------------
void IntVec2::Rotate90Degrees()
{
	int buffer = x;
	x = -y;
	y = buffer;
}

//-----------------------------------------------------------------------------------------------
void IntVec2::RotateMinus90Degrees()
{
	int buffer = x;
	x = y;
	y = -buffer;
}

//-----------------------------------------------------------------------------------------------
void IntVec2::SetFromText( char const* text )
{
	Strings splitVec2 = SplitStringOnDelimiter( text, ',' );
	x = atoi( splitVec2[0].c_str() );
	y = atoi( splitVec2[1].c_str() );
}

//-----------------------------------------------------------------------------------------------
void IntVec2::operator=(const IntVec2& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
}

bool IntVec2::operator==( IntVec2 const& compare ) const
{
	return x == compare.x && y == compare.y;
}

//-----------------------------------------------------------------------------------------------
IntVec2 const IntVec2::operator+( IntVec2 const& vecToAdd ) const
{
	return IntVec2( x + vecToAdd.x, y + vecToAdd.y);
}
