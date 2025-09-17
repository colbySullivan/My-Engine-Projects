#include "Engine/Math/IntVec2.hpp"
#include<math.h>
#include "MathUtils.hpp"

IntVec2::IntVec2(IntVec2 const& copyFrom)
	: x(copyFrom.x)
	, y(copyFrom.y)
{

}

IntVec2::IntVec2(int initialX, int initialY)
	: x(initialX)
	, y(initialY)
{

}

float IntVec2::GetLength() const
{
	return static_cast<float>(sqrt((x * x) + (y * y)));
}

int IntVec2::GetTaxicabLength() const
{
	return abs(x) + abs(y);
}

int IntVec2::GetLengthSquared() const
{
	return (x * x) + (y * y);
}

float IntVec2::GetOrientationRadians() const
{
	return static_cast<float>(atan2(y, x));
}

float IntVec2::GetOrientationDegrees() const
{
	return ConvertRadiansToDegrees(static_cast<float>(atan2(y, x)));
}

IntVec2 const IntVec2::GetRotatedBy90Degrees() const
{
	return IntVec2( -y, x );
}

IntVec2 const IntVec2::GetRotatedByMinus90Degrees() const
{
	return IntVec2( y, -x );
}

void IntVec2::Rotate90Degrees()
{
	int buffer = x;
	x = -y;
	y = buffer;
}

void IntVec2::RotateMinus90Degrees()
{
	int buffer = x;
	x = y;
	y = -buffer;
}

void IntVec2::operator=(const IntVec2& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
}