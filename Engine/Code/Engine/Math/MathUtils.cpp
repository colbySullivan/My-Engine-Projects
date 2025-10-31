#include "MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/AABB2.hpp"
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

	while (angularDisp > 180.0f) 
    {
		angularDisp -= 360.0f;
	}

	while (angularDisp <= -180.0f) 
    { // (-180.f,180.f]
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

	while (result >= 360.0f) 
    {
		result -= 360.0f;
	}

	while (result < 0.0f) 
    {
		result += 360.0f;
	}

	return result;

}

float GetAngleDegreesBetweenVectors2D( Vec2 const& a, Vec2 const& b )
{
	float angleCos = DotProduct2D( a, b) / ( a.GetLength() * b.GetLength() );
	angleCos = GetClamped( angleCos, -1.f, 1.f );
	float angleRad = acosf( angleCos );
	return ConvertRadiansToDegrees( angleRad );
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

int GetTaxicabDistance2D( IntVec2 const& pointA, IntVec2 const& pointB )
{
	return abs( pointB.x - pointA.x ) + abs( pointB.y - pointA.y );
}

//-----------------------------------------------------------------------------------------------
float GetProjectedLength2D( Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto )
{
    return DotProduct2D( vectorToProject, vectorToProjectOnto.GetNormalized() );
}

//-----------------------------------------------------------------------------------------------
Vec2 GetProjectedVector2D( Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto )
{
    float projectedLength = GetProjectedLength2D( vectorToProject, vectorToProjectOnto );
    return ( vectorToProjectOnto.GetNormalized() * projectedLength );
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

bool IsPointInsideDisc2D( Vec2 const& point, Vec2 const& discCenter, float discRadius )
{
	Vec2 centerToPoint = discCenter - point;
	float overlapDist = discRadius - centerToPoint.GetLength();
	return ( overlapDist > 0 );
}

bool IsPointInsideOrientedSector2D( Vec2 const& point, Vec2 const& sectorTip, float sectorFwdDegrees, float sectorApertureDegrees, float sectorRadius )
{
	if ( !IsPointInsideDisc2D( point, sectorTip, sectorRadius ) )
		return false;

	Vec2 sectToPoint = point - sectorTip;
	float displacementDegrees = GetShortestAngularDispDegrees( sectToPoint.GetOrientationDegrees(), sectorFwdDegrees );
	if ( fabsf( displacementDegrees ) > sectorApertureDegrees * 0.5f )
		return false;

	return true;
}

bool IsPointInsideDirectedSector2D( Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorFwdNormal, float sectorApertureDegrees, float sectorRadius )
{
	if ( !IsPointInsideDisc2D( point, sectorTip, sectorRadius ) )
		return false;

	Vec2 sectToPoint = point - sectorTip;
	float sectorFwdDegrees = sectorFwdNormal.GetOrientationDegrees();
	float displacementDegrees = GetShortestAngularDispDegrees( sectToPoint.GetOrientationDegrees(), sectorFwdDegrees );
	if ( fabsf( displacementDegrees ) > sectorApertureDegrees * 0.5f )
		return false;

	return true;
}

Vec2 GetNearestPointOnDisc2D( Vec2 const& referencePos, Vec2 const& discCenter, float discRadius )
{
	Vec2 centerToPoint = referencePos - discCenter; 
	centerToPoint.ClampLength( discRadius );
	return discCenter + centerToPoint;
}

bool PushDiscOutOfFixedPoint2D( Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedPoint )
{
	if ( IsPointInsideDisc2D(fixedPoint, mobileDiscCenter, discRadius) )
	{
		Vec2 centerToPoint = mobileDiscCenter - fixedPoint;
		float overlapDist = discRadius - centerToPoint.GetLength();

        mobileDiscCenter += centerToPoint.GetNormalized() * overlapDist;
		return true;
	}

	return false;
}

bool PushDiscOutOfFixedDisc2D( Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius )
{
	if( DoDiscsOverlap( mobileDiscCenter, discRadius, fixedDiscCenter, fixedDiscRadius) )
	{
		Vec2 centerToCenter = mobileDiscCenter - fixedDiscCenter;
		float centerToCenterLength = centerToCenter.GetLength();
		float discSeparation = discRadius + fixedDiscRadius;
		float overlapDist = discSeparation - centerToCenterLength;
		mobileDiscCenter += centerToCenter.GetNormalized() * overlapDist;

		return true;
	}

	return false;
}

bool PushDiscsOutOfEachOther2D( Vec2& aCenter, float aRadius, Vec2& bCenter, float bRadius )
{
	if ( DoDiscsOverlap( aCenter, aRadius, bCenter, bRadius ) )
	{
		Vec2 centerToCenter = bCenter - aCenter;
		float centerToCenterLength = centerToCenter.GetLength();
		float discSepDist = aRadius + bRadius;
		float overlapDist = discSepDist - centerToCenterLength;

		Vec2 pushDirection = centerToCenter.GetNormalized();
		float halfOverlap = overlapDist * 0.5f;
		aCenter -= pushDirection * halfOverlap;
		bCenter += pushDirection * halfOverlap;

		return true;
	}

	return false;
}

bool PushDiscOutOfFixedAABB2D( Vec2& mobileDiscCenter, float discRadius, AABB2 const& fixedBox )
{
	Vec2 nearestPoint = fixedBox.GetNearestPoint( mobileDiscCenter );
	return PushDiscOutOfFixedPoint2D( mobileDiscCenter, discRadius, nearestPoint );
}

//-----------------------------------------------------------------------------------------------
void TransformPosition2D(Vec2& posToTransform, float uniformScale, float rotationDegrees, Vec2 const& translation)
{
    posToTransform *= uniformScale;
    posToTransform.RotateDegrees(rotationDegrees);
    posToTransform += translation;
}

//-----------------------------------------------------------------------------------------------
void TransformPosition2D( Vec2& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation )
{
	posToTransform = ( iBasis * posToTransform.x ) + ( jBasis * posToTransform.y ) + translation;
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

//-----------------------------------------------------------------------------------------------
void TransformPositionXY3D( Vec3& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation )
{
	float originalX = posToTransform.x;
	float originalY = posToTransform.y;
	posToTransform.x = ( iBasis.x * originalX ) + ( jBasis.x * originalY ) + translation.x;
	posToTransform.y = ( iBasis.y * originalX ) + ( jBasis.y * originalY ) + translation.y;
}

//------------------------------------------------------------------------------
bool IsPointInsideOBB2D(Vec2 point, OBB2 const& orientedBox)
{
	Vec2 localPos = orientedBox.GetLocalPosForWorldPos(point);
	Vec2 halfDimensions = orientedBox.m_halfDimensions;

	if (localPos.x >= -halfDimensions.x && localPos.x <= halfDimensions.x &&
		localPos.y >= -halfDimensions.y && localPos.y <= halfDimensions.y)
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnInfiniteLine2D( Vec2 referencePos, Vec2 pointOnLine, Vec2 anotherPointOnLine )
{
	Vec2 pointDirection = pointOnLine - referencePos;
	Vec2 SE = anotherPointOnLine - referencePos;
	Vec2 SN = GetProjectedVector2D(pointDirection, SE);
	return pointOnLine + SN;
}

//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnLineSegment2D( Vec2 referencePos, Vec2 start, Vec2 end )
{
	Vec2 startDirection = end - start;
	Vec2 pointDirection = referencePos - start;
	Vec2 endDirection = referencePos - end;

	if ( DotProduct2D( startDirection, pointDirection ) <= 0 )
	{
		return start;
	}

	if ( DotProduct2D( endDirection, startDirection ) > 0 )
	{
		return end;
	}

	Vec2 SN = GetProjectedVector2D( pointDirection, startDirection );
	return start + SN;
}

////-----------------------------------------------------------------------------------------------
//Vec2 GetNearestPointOnTriangle2D( Vec2 referencePos, Vec2 ccw0, Vec2 ccw1, Vec2 ccw2 )
//{
//	//check inside
//
//	//nearestpointAB
//	//nearestpointBC
//	//nearestpointCA
//
//	// float distSqAB , BC , CA GetDistanceSq
//
//	// if distsqAB <= distsq BC && distSQab <= distSqCa return nearestPointAB
//	// else if BC <= CA return BC
//	// else return CA
//
//}

