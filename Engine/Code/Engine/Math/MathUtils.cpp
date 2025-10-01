#include "MathUtils.hpp"
#include "Vec2.hpp"
#include "Vec3.hpp"
#include<math.h>

float GetClamped(float value, float minValue, float maxValue)
{
    if(value <= minValue)
        return minValue;
	if (value >= maxValue)
		return maxValue;
    return value;
}

//-----------------------------------------------------------------------------------------------
float GetClampedZeroToOne(float value)
{
	if (value <= 0)
		return 0;
	if (value >= 1)
		return 1;
	return value;
}

//-----------------------------------------------------------------------------------------------
float Interpolate(float start, float end, float fractionTowardEnd)
{
    return ((1-fractionTowardEnd) * start) + (fractionTowardEnd * end);
}

//-----------------------------------------------------------------------------------------------
float GetFractionWithinRange(float value, float rangeStart, float rangeEnd)
{
    if(rangeStart == rangeEnd)
        return 0.5f;

    return (value - rangeStart) / (rangeEnd - rangeStart);
}

//-----------------------------------------------------------------------------------------------
float RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{
    return Interpolate(outStart, outEnd, GetFractionWithinRange(inValue, inStart, inEnd));
}

//-----------------------------------------------------------------------------------------------
float RangeMapClamped(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{
    float clampedFraction = GetClampedZeroToOne(GetFractionWithinRange(inValue, inStart, inEnd));
    return Interpolate(outStart, outEnd, clampedFraction);
}

//-----------------------------------------------------------------------------------------------
int RoundDownToInt(float value)
{
    return static_cast<int>(floorf(value));
}

//-----------------------------------------------------------------------------------------------
float ConvertDegreesToRadians(float degrees)
{
    return degrees * static_cast<float>( M_PI / 180.f );
}

//-----------------------------------------------------------------------------------------------
float ConvertRadiansToDegrees(float radians)
{
    return radians *  static_cast<float>( 180.f / M_PI );;
}

//-----------------------------------------------------------------------------------------------
float CosDegrees( float degrees )
{
    return cosf( ConvertDegreesToRadians( degrees ) );
}

//-----------------------------------------------------------------------------------------------
float SinDegrees(float degrees)
{
    return sinf( ConvertDegreesToRadians( degrees ) );
}

//-----------------------------------------------------------------------------------------------
float Atan2Degrees(float y, float x)
{
	float radians = atan2f(y, x);
	return radians *  static_cast<float>(180.0f / M_PI);
}

//-----------------------------------------------------------------------------------------------
float GetShortestAngularDispDegrees(float startDegrees, float endDegrees)
{
	float angularDisp = endDegrees - startDegrees;

	while (angularDisp > 180.0f) {
		angularDisp -= 360.0f;
	}
	while (angularDisp <= -180.0f) { // (-180.f,180.f]
		angularDisp += 360.0f;
	}

	return angularDisp;
}

//-----------------------------------------------------------------------------------------------
float GetTurnedTowardDegrees(float currentDegrees, float goalDegrees, float maxDeltaDegrees)
{
	float angularDisp = GetShortestAngularDispDegrees(currentDegrees, goalDegrees);
	float clampedDisp = GetClamped(angularDisp, -maxDeltaDegrees, maxDeltaDegrees);
	float result = currentDegrees + clampedDisp;

	while (result >= 360.0f) {
		result -= 360.0f;
	}
	while (result < 0.0f) {
		result += 360.0f;
	}

	return result;

}

//-----------------------------------------------------------------------------------------------
float DotProduct2D(Vec2 const& a, Vec2 const& b)
{
    return (a.x * b.x) + (a.y * b.y);
}

//-----------------------------------------------------------------------------------------------
float GetDistance2D(Vec2 const& positionA, Vec2 const& positionB)
{
    return sqrtf(GetDistanceSquared2D( positionA, positionB ));
}

//-----------------------------------------------------------------------------------------------
float GetDistanceSquared2D(Vec2 const& positionA, Vec2 const& positionB)
{
   return ((positionB.x - positionA.x) * (positionB.x - positionA.x))
        +  ((positionB.y - positionA.y) * (positionB.y - positionA.y));
}

//-----------------------------------------------------------------------------------------------
float GetDistance3D(Vec3 const& positionA, Vec3 const& positionB)
{
    return sqrtf(GetDistanceSquared3D(positionA, positionB));
}

//-----------------------------------------------------------------------------------------------
float GetDistanceSquared3D(Vec3 const& positionA, Vec3 const& positionB)
{
    return ((positionB.x - positionA.x) * (positionB.x - positionA.x))
        + ((positionB.y - positionA.y) * (positionB.y - positionA.y))
        + ((positionB.z - positionA.z) * (positionB.z - positionA.z));
}

//-----------------------------------------------------------------------------------------------
float GetDistanceXY3D(Vec3 const& positionA, Vec3 const& positionB)
{
    return sqrtf(GetDistanceXYSquared3D(positionA, positionB));
}

//-----------------------------------------------------------------------------------------------
float GetDistanceXYSquared3D(Vec3 const& positionA, Vec3 const& positionB)
{
    return ((positionB.x - positionA.x) * (positionB.x - positionA.x))
        + ((positionB.y - positionA.y) * (positionB.y - positionA.y));
}

//-----------------------------------------------------------------------------------------------
bool DoDiscsOverlap(Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB)
{
    float distance = GetDistanceSquared2D(centerA, centerB);
    float radiusSum = (radiusA + radiusB) * (radiusA + radiusB);
    return distance <= radiusSum;
}

//-----------------------------------------------------------------------------------------------
bool DoSpheresOverlap(Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB)
{
    float distance = GetDistanceSquared3D(centerA, centerB);
    float radiusSum = (radiusA + radiusB) * (radiusA + radiusB);
    return distance <= radiusSum;
}

//-----------------------------------------------------------------------------------------------
void TransformPosition2D(Vec2& posToTransform, float uniformScale, float rotationDegrees, Vec2 const& translation)
{
    posToTransform *= uniformScale;
    posToTransform.RotateDegrees(rotationDegrees);
    posToTransform += translation;
}

//-----------------------------------------------------------------------------------------------
void TransformPositionXY3D(Vec3& posToTransform, float xyScale, float zRotationDegrees, Vec2 const& xyTranslation)
{
	posToTransform.x *= xyScale;
    posToTransform.y *= xyScale;
	posToTransform = posToTransform.GetRotatedAboutZDegrees(zRotationDegrees);
	posToTransform.x += xyTranslation.x;
    posToTransform.y += xyTranslation.y;
}

