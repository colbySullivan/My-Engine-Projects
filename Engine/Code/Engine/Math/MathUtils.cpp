#include "MathUtils.hpp"
#include "Vec2.hpp"
#include "Vec3.hpp"
#include<math.h>

float ConvertDegreesToRadians(float degrees)
{
    return degrees * ( M_PI / 180.f );
}

float ConvertRadiansToDegrees(float radians)
{
    return radians * ( 180.f / M_PI );;
}

float CosDegrees( float degrees )
{
    return cosf( ConvertDegreesToRadians( degrees ) );
}

float SinDegrees(float degrees)
{
    return sinf( ConvertDegreesToRadians( degrees ) );
}

float Atan2Degrees(float y, float x)
{
	float radians = atan2f(y, x);
	return radians * (180.0f / M_PI);
}

float GetDistance2D(Vec2 const& positionA, Vec2 const& positionB)
{
    return sqrtf(GetDistanceSquared2D( positionA, positionB ));
}

float GetDistanceSquared2D(Vec2 const& positionA, Vec2 const& positionB)
{
   return ((positionB.x - positionA.x) * (positionB.x - positionA.x))
        +  ((positionB.y - positionA.y) * (positionB.y - positionA.y));
}

float GetDistance3D(Vec3 const& positionA, Vec3 const& positionB)
{
    return sqrtf(GetDistanceSquared3D(positionA, positionB));
}

float GetDistanceSquared3D(Vec3 const& positionA, Vec3 const& positionB)
{
    return ((positionB.x - positionA.x) * (positionB.x - positionA.x))
        + ((positionB.y - positionA.y) * (positionB.y - positionA.y))
        + ((positionB.z - positionA.z) * (positionB.z - positionA.z));
}

float GetDistanceXY3D(Vec3 const& positionA, Vec3 const& positionB)
{
    return sqrtf(GetDistanceXYSquared3D(positionA, positionB));
}

float GetDistanceXYSquared3D(Vec3 const& positionA, Vec3 const& positionB)
{
    return ((positionB.x - positionA.x) * (positionB.x - positionA.x))
        + ((positionB.y - positionA.y) * (positionB.y - positionA.y));
}

bool DoDiscsOverlap(Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB)
{
    float distance = GetDistanceSquared2D(centerA, centerB);
    float radiusSum = (radiusA + radiusB) * (radiusA + radiusB);
    return distance <= radiusSum;
}

bool DoSpheresOverlap(Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB)
{
    float distance = GetDistanceSquared3D(centerA, centerB);
    float radiusSum = (radiusA + radiusB) * (radiusA + radiusB);
    return distance <= radiusSum;
}

void TransformPosition2D(Vec2& posToTransform, float uniformScale, float rotationDegrees, Vec2 const& translation)
{
    posToTransform *= uniformScale;
    posToTransform.RotateDegrees(rotationDegrees);
    posToTransform += translation;
}

void TransformPositionXY3D(Vec3& posToTransform, float xyScale, float zRotationDegrees, Vec2 const& xyTranslation)
{
	posToTransform.x *= xyScale;
    posToTransform.y *= xyScale;
	posToTransform = posToTransform.GetRotatedAboutZDegrees(zRotationDegrees);
	posToTransform.x += xyTranslation.x;
    posToTransform.y += xyTranslation.y;
}

