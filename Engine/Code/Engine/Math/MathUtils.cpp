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
    return cosf( degrees );
}

float SinDegrees(float degrees)
{
    return sinf( degrees );
}

float Atan2Degrees(float y, float x)
{
    return atan2f( y, x );
}

float GetDistance2D(Vec2 const& positionA, Vec2 const& positionB)
{
   /*return sqrt((( positionB.x - positionA.x ) * ( positionB.x - positionA.x )) 
              + (( positionB.y - positionA.y ) * ( positionB.y - positionA.y )));*/
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

// ToDo rewrite this
void TransformPosition2D(Vec2& posToTransform, float uniformScale, float rotationDegrees, Vec2 const& translation)
{
    // Step 1: Apply uniform scale
    posToTransform.x *= uniformScale;
    posToTransform.y *= uniformScale;

    // Step 2: Apply rotation
    if (rotationDegrees != 0.0f)
    {
        // Convert degrees to radians
        float rotationRadians = rotationDegrees * (3.14159265359f / 180.0f);

        // Calculate sin and cos
        float cosTheta = cosf(rotationRadians);
        float sinTheta = sinf(rotationRadians);

        // Apply rotation matrix
        float newX = posToTransform.x * cosTheta - posToTransform.y * sinTheta;
        float newY = posToTransform.x * sinTheta + posToTransform.y * cosTheta;

        posToTransform.x = newX;
        posToTransform.y = newY;
    }

    // Step 3: Apply translation
    posToTransform.x += translation.x;
    posToTransform.y += translation.y;
}

void TransformPositionXY3D(Vec3& posToTransform, float xyScale, float zRotationDegrees, Vec2 const& xyTranslation)
{
}

