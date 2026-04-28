#include "MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/Mat44.hpp"
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
    //return ((1-fractionTowardEnd) * start) + (fractionTowardEnd * end);
	return start + (fractionTowardEnd * (end - start)); // Easier to read
}

//-----------------------------------------------------------------------------------------------
float InterpolateShortestDis(float start, float end, float fractionTowardEnd)
{
	float delta = GetShortestAngularDispDegrees(start, end);
	return start + (fractionTowardEnd * delta);
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
float NormalizeByte( unsigned char byte )
{
	return byte / 255.f;
}

//-----------------------------------------------------------------------------------------------
unsigned char DenormalizeByte( float value )
{
	if ( value >= 1.f )
	{
		return 255;
	}
	value = GetClampedZeroToOne(value);
	int byteValue = static_cast<int>(value * 256.f);

	return static_cast<unsigned char>(byteValue);
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

//------------------------------------------------------------------------------
float DotProduct3D(Vec3 const& a, Vec3 const& b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

//-----------------------------------------------------------------------------------------------
float DotProduct4D( Vec4 const& a, Vec4 const& b )
{
	return ( a.x * b.x ) + ( a.y * b.y ) + ( a.z * b.z ) + ( a.w * b.w );
}


//------------------------------------------------------------------------------
float CrossProduct2D(Vec2 const& a, Vec2 const& b)
{
	return (a.x * b.y) - (a.y * b.x);
}

//------------------------------------------------------------------------------
Vec3 CrossProduct3D(Vec3 const& a, Vec3 const& b)
{
	float x = (a.y * b.z) - (a.z * b.y);
	float y = (a.z * b.x) - (a.x * b.z);
	float z = (a.x * b.y) - (a.y * b.x);
	return Vec3(x, y, z);
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

//-----------------------------------------------------------------------------------------------
bool DoAABB2sOverlap( AABB2 const& alignedBoxA, AABB2 const& alignedBoxB )
{
	if ( alignedBoxA.m_maxs.x < alignedBoxB.m_mins.x || alignedBoxB.m_maxs.x < alignedBoxA.m_mins.x )
	{
		return false;
	}
	if ( alignedBoxA.m_maxs.y < alignedBoxB.m_mins.y || alignedBoxB.m_maxs.y < alignedBoxA.m_mins.y )
	{
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
bool DoCylindersOverlap( const Vec3& startA, const Vec3& endA, float radiusA, const Vec3& startB, const Vec3& endB, float radiusB )
{
	FloatRange rangeA( startA.z, endA.z );
	FloatRange rangeB( startB.z, endB.z );
	if ( rangeA.IsOverlappingWith( rangeB ) )
	{
		Vec2 centerA( startA.x, startA.y );
		Vec2 centerB( startB.x, startB.y );
		return DoDiscsOverlap( centerA, radiusA, centerB, radiusB );
	}
	return false;

}

//------------------------------------------------------------------------------
bool DoSphereABB3Overlap( Vec3 const& sphereCenter, float sphereRadius, Vec3 const& boxMins, Vec3 const& boxMaxs )
{
	Vec3 nearestPoint = GetNearestPointOnAABB3D( sphereCenter, AABB3( boxMins, boxMaxs ) );
	float dist = GetDistance3D( sphereCenter, nearestPoint );
	return dist < sphereRadius;
}

//------------------------------------------------------------------------------
bool DoSphereCylinderOverlap( Vec3 const& sphereCenter, float sphereRadius, const Vec3& start, const Vec3& end, float radius )
{
	if ( DoDiscsOverlap( Vec2( sphereCenter.x, sphereCenter.y ), sphereRadius, Vec2( start.x, start.y ), radius ) )
	{
		return ( sphereCenter.z >= start.z - sphereRadius ) && ( sphereCenter.z <= end.z + sphereRadius );
	}
	return false;
}

//------------------------------------------------------------------------------
bool DoAABB3sOverlap( const Vec3& minsA, const Vec3& maxsA, const Vec3& minsB, const Vec3& maxsB )
{
	if ( minsA.x >= maxsB.x || minsB.x >= maxsA.x ) return false;
	if ( minsA.y >= maxsB.y || minsB.y >= maxsA.y ) return false;
	if ( minsA.z >= maxsB.z || minsB.z >= maxsA.z ) return false;
	return true;
}

//------------------------------------------------------------------------------
bool DoAABB3sCylinderOverlap( const Vec3& minsA, const Vec3& maxsA, Vec2 cylinderCenter, float cylinderRadius, FloatRange cylinderZRange )
{
	FloatRange rangeA( minsA.z, maxsA.z );
	if ( rangeA.IsOverlappingWith( cylinderZRange ) )
	{
		Vec2 centerA( minsA.x + (maxsA.x - minsA.x) * 0.5f, minsA.y + (maxsA.y - minsA.y) * 0.5f );
		return DoDiscsOverlap( centerA, GetDistance2D( Vec2(minsA.x, minsA.y), Vec2(maxsA.x, maxsA.y) ) * 0.5f, cylinderCenter, cylinderRadius );
	}
	return false;
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

Vec3 GetNearestPointOnAABB3D( Vec3 const& referencePos, const AABB3& bounds )
{
	Vec3 nearestPoint;
	nearestPoint.x = GetClamped( referencePos.x, bounds.m_mins.x, bounds.m_maxs.x );
	nearestPoint.y = GetClamped( referencePos.y, bounds.m_mins.y, bounds.m_maxs.y );
	nearestPoint.z = GetClamped( referencePos.z, bounds.m_mins.z, bounds.m_maxs.z );

	return nearestPoint;
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

//------------------------------------------------------------------------------
bool PushCylinderOutOfFixedCylinder( Vec3& mobileStart, Vec3& mobileEnd, float mobileRadius, const Vec3& fixedStart, const Vec3& fixedEnd, float fixedRadius )
{
	FloatRange mobileZRange( mobileStart.z, mobileEnd.z );
	FloatRange fixedZRange( fixedStart.z, fixedEnd.z );
	if ( mobileZRange.IsOverlappingWith( fixedZRange ) )
	{
		Vec2 mobileCenter( mobileStart.x, mobileStart.y );
		Vec2 fixedCenter( fixedStart.x, fixedStart.y );
		if ( PushDiscOutOfFixedDisc2D( mobileCenter, mobileRadius, fixedCenter, fixedRadius ) )
		{
			mobileStart.x = mobileCenter.x;
			mobileStart.y = mobileCenter.y;
			mobileEnd.x = mobileCenter.x;
			mobileEnd.y = mobileCenter.y;
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------
bool PushCylindersOutOfEachOther( Vec3& aStart, Vec3& aEnd, float aRadius, Vec3& bStart, Vec3& bEnd, float bRadius )
{
	FloatRange aZRange( aStart.z, aEnd.z );
	FloatRange bZRange( bStart.z, bEnd.z );
	if ( !aZRange.IsOverlappingWith( bZRange ) )
	{
		return false;
	}

	Vec2 aCenter( aStart.x, aStart.y );
	Vec2 bCenter( bStart.x, bStart.y );

	if ( !PushDiscsOutOfEachOther2D( aCenter, aRadius, bCenter, bRadius ) )
	{
		return false;
	}

	aStart.x = aCenter.x;
	aStart.y = aCenter.y;
	aEnd.x = aCenter.x;
	aEnd.y = aCenter.y;

	bStart.x = bCenter.x;
	bStart.y = bCenter.y;
	bEnd.x = bCenter.x;
	bEnd.y = bCenter.y;

	return true;
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

bool IsPointInsideAABB2D(Vec2 point, AABB2 const& alignedBox)
{
	return alignedBox.IsPointInside(point);
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
bool IsPointInsideCapsule2D( Vec2 point, Vec2 boneStart, Vec2 boneEnd, float radius )
{
	Vec2 distAB = boneEnd - boneStart;
	Vec2 sect1 =  boneStart - point;
	float aSection = DotProduct2D( sect1, distAB.GetNormalized() );

	Vec2 distBA = boneStart - boneEnd;
	Vec2 sect2 = boneEnd - point;
	float bSection = DotProduct2D( sect2, distBA.GetNormalized() );

	float cSection = DotProduct2D( distAB.GetRotatedBy90Degrees().GetNormalized(), sect1 );
	float dSection = DotProduct2D( distAB.GetRotatedByMinus90Degrees().GetNormalized(), sect1 );

	return (aSection <= radius && bSection <= radius && cSection <= radius && dSection <=radius);
}

//-----------------------------------------------------------------------------------------------
bool IsPointInsideTriangle2D( Vec2 point, Vec2 ccw0, Vec2 ccw1, Vec2 ccw2 )
{
	Vec2 distAB = ccw1 - ccw0;
	Vec2 distPointA = ccw0 - point;
	float aSection = DotProduct2D(distAB.GetRotatedBy90Degrees(), distPointA);

	Vec2 distBC = ccw2 - ccw1;
	Vec2 distPointB = ccw1 - point;
	float bSection = DotProduct2D( distBC.GetRotatedBy90Degrees(), distPointB );

	Vec2 distCA = ccw0 - ccw2;
	Vec2 distPointC = ccw2 - point;
	float cSection = DotProduct2D( distCA.GetRotatedBy90Degrees(), distPointC );

	return ( aSection <= 0 && bSection <= 0 && cSection <= 0  );

}

//------------------------------------------------------------------------------
bool IsPointInsideAABB3( Vec3 startPos, Vec3 boxMins, Vec3 boxMaxs )
{
	return ( startPos.x >= boxMins.x && startPos.x <= boxMaxs.x &&
			 startPos.y >= boxMins.y && startPos.y <= boxMaxs.y &&
			 startPos.z >= boxMins.z && startPos.z <= boxMaxs.z );
}

//-----------------------------------------------------------------------------------------------
RaycastResult2D RaycastVsDisc2D( Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 discCenter, float discRadius )
{
	RaycastResult2D result;
	Vec2 const& i = fwdNormal;
	Vec2 j = fwdNormal.GetRotatedBy90Degrees();
	Vec2 SC = discCenter - startPos;
	float SCj = DotProduct2D( SC, j );

	if ( SCj >= discRadius )
	{
		return result; // Too far to the left
	}
	if ( SCj <= -discRadius )
	{
		return result; // Too far to the right
	}

	float SCi = DotProduct2D( SC, i );
	if ( SCi >= maxDist + discRadius )
	{
		return result; // Too late; disc is after ray end
	}
	if ( SCi <= -discRadius )
	{
		return result; // Too early; disc is before ray start
	}

	float adjustmentDistance = sqrtf( ( discRadius * discRadius ) - ( SCj * SCj ) );
	float impactDist = SCi - adjustmentDistance;
	result.m_exitDist = SCi + adjustmentDistance;

	if ( IsPointInsideDisc2D( startPos, discCenter, discRadius ) )
	{
		// Hit started inside the disc
		result.m_didImpact = true;
		result.m_impactDist = 0.f;
		result.m_impactPos = startPos;
		result.m_impactNormal = -fwdNormal;
		return result;
	}

	if ( impactDist >= maxDist )
	{
		return result; // Too late; impact point on dic would be after end of ray 
	}
	if ( impactDist <= 0.f )
	{
		return result; // Too early; impact point on dic would be before start of ray 
	}

	result.m_didImpact = true;
	result.m_impactDist = impactDist;
	result.m_impactPos = startPos + ( fwdNormal * result.m_impactDist);
	result.m_impactNormal = (result.m_impactPos - discCenter).GetNormalized();
	return result;
}

//-----------------------------------------------------------------------------------------------
RaycastResult2D RaycastVsLine2D( Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 pointA, Vec2 pointB )
{
	RaycastResult2D result;
	Vec2 const& i = fwdNormal;
	Vec2 j = fwdNormal.GetRotatedBy90Degrees();

	Vec2 RayToA = pointA - startPos;
	Vec2 RayToB = pointB - startPos;

	float RayToAOntoJ = DotProduct2D(RayToA, j);
	float RayToBOntoJ = DotProduct2D(RayToB, j);
	if ( (RayToAOntoJ * RayToBOntoJ) >= 0 )
	{
		return result;
	}

	float RayToAOntoI = DotProduct2D( RayToA, i );
	float RayToBOntoI = DotProduct2D( RayToB, i );

	if ( RayToAOntoI <= 0 && RayToBOntoI <= 0 )
	{
		return result;
	}
	if ( RayToAOntoI >= maxDist && RayToBOntoI >= maxDist )
	{
		return result;
	}

	float lineSegHitFraction = RayToAOntoJ / ( RayToAOntoJ - RayToBOntoJ );
	result.m_impactDist = RayToAOntoI + ( lineSegHitFraction * ( RayToBOntoI - RayToAOntoI ) );

	if ( result.m_impactDist <= 0 || result.m_impactDist >= maxDist )
	{
		return result;
	}

	result.m_impactPos = startPos + ( i * result.m_impactDist );
	result.m_impactNormal = pointB - pointA;
	result.m_impactNormal.Rotate90Degrees();
	result.m_impactNormal.Normalize();
	if ( DotProduct2D( result.m_impactNormal, i ) > 0 )
	{
		result.m_impactNormal *= -1;
	}

	result.m_didImpact = true;
	return result;
}

//-----------------------------------------------------------------------------------------------
RaycastResult2D RaycastVsAABB22D( Vec2 startPos, Vec2 fwdNormal, float maxDist, AABB2& box )
{
	RaycastResult2D result;

	if ( IsPointInsideAABB2D( startPos, box ) )
	{
		result.m_didImpact = true;
		result.m_impactDist = 0.f;
		result.m_impactPos = startPos;
		result.m_impactNormal = -fwdNormal;
		return result;
	}

	Vec2 rayEndPoint = startPos + (fwdNormal * maxDist);

	AABB2 rayBox = AABB2( Vec2( GetFloatMin( rayEndPoint.x, startPos.x), GetFloatMin( rayEndPoint.y, startPos.y) ), Vec2( GetFloatMax( rayEndPoint.x, startPos.x), GetFloatMax( rayEndPoint.y, startPos.y) ) );
	if ( !DoAABB2sOverlap( rayBox, box ) )
	{
		return result;
	}

	float xLength = rayEndPoint.x - startPos.x;
	float tMinX = ( box.m_mins.x - startPos.x ) / xLength;
	float tMaxX = ( box.m_maxs.x - startPos.x ) / xLength;

	float yLength = rayEndPoint.y - startPos.y;
	float tMinY = ( box.m_mins.y - startPos.y ) / yLength;
	float tMaxY = ( box.m_maxs.y - startPos.y ) / yLength;

	if ( tMinX > tMaxX )
	{
		float temp = tMinX;
		tMinX = tMaxX;
		tMaxX = temp;
	}

	if ( tMinY > tMaxY )
	{
		float temp = tMinY;
		tMinY = tMaxY;
		tMaxY = temp;
	}

	FloatRange xRange = FloatRange(tMinX, tMaxX);
	FloatRange yRange = FloatRange(tMinY, tMaxY);

	if ( !xRange.IsOverlappingWith( yRange ) )
	{
		return result;
	}

	float tEnter = tMinX;
	if ( tMinY > tMinX )
	{
		tEnter = tMinY;
	}

	float tExit = tMaxX;
	if ( tMaxY < tMaxX )
	{
		tExit = tMaxY;
	}

	if ( tEnter > tExit || tEnter >= 1.f || tExit <= 0.f )
	{
		return result;
	}

	result.m_didImpact = true;
	result.m_impactDist = tEnter * maxDist;
	result.m_impactPos = startPos + ( fwdNormal * result.m_impactDist );

	if ( tMinX > tMinY )
	{
		if ( fwdNormal.x < 0.f )
		{
			result.m_impactNormal = Vec2( 1.f, 0.f );
		}
		else
		{
			result.m_impactNormal = Vec2( -1.f, 0.f );
		}
	}
	else
	{
		if ( fwdNormal.y < 0.f )
		{
			result.m_impactNormal = Vec2( 0.f, 1.f );
		}
		else
		{
			result.m_impactNormal = Vec2( 0.f, -1.f );
		}
	}
	return result;
}

//------------------------------------------------------------------------------
RaycastResult3D RaycastVsCylinder( Vec3 startPos, Vec3 fwdNormal, float maxDist, Vec3 cylinderStart, Vec3 cylinderEnd, float radius )
{
	RaycastResult3D result;
	fwdNormal = fwdNormal.GetNormalized();

	Vec2 cylPosXY( cylinderStart.x, cylinderStart.y );

	// Ray intersects on the XY plane
	Vec2 rayFwd2D( fwdNormal.x, fwdNormal.y );
	Vec2 rayStart2D( startPos.x, startPos.y );
	float endX  = startPos.x + ( fwdNormal.x * maxDist );
	float endY  = startPos.y + ( fwdNormal.y * maxDist );
	Vec2 rayEnd2D = Vec2( endX, endY );
	float rayLength = ( rayEnd2D -  rayStart2D).GetLength();
	rayFwd2D = rayFwd2D.GetNormalized();
	RaycastResult2D hit2D = RaycastVsDisc2D( rayStart2D, rayFwd2D, rayLength, cylPosXY, radius );

	if ( !hit2D.m_didImpact )
	{
		return result;
	}

	float tDiscEnter = hit2D.m_impactDist / rayLength;
	float tDiscExit = hit2D.m_exitDist / rayLength;
	FloatRange tSideRange( fminf(tDiscEnter, tDiscExit), fmaxf(tDiscEnter, tDiscExit) );

	float zStep = fwdNormal.z * maxDist;
	float cylMinZ = fminf( cylinderStart.z, cylinderEnd.z );
	float cylMaxZ = fmaxf( cylinderStart.z, cylinderEnd.z );
	float tAtMinZ = ( cylMinZ - startPos.z ) / zStep;
	float tAtMaxZ = ( cylMaxZ - startPos.z ) / zStep;

	FloatRange tCylRange = FloatRange( fminf( tAtMinZ, tAtMaxZ ), fmaxf( tAtMinZ, tAtMaxZ ) );

	// Ray intersects on Z and XY within range
	if ( !tSideRange.IsOverlappingWith( tCylRange ) )
	{
		return result;
	}

	// Get max of min for intersect T
	float tEnter = fmaxf( tSideRange.m_min, tCylRange.m_min );
	float tExit = fminf( tSideRange.m_max, tCylRange.m_max );

	if ( tEnter > 1.f || tExit < 0.f )
	{
		return result;
	}

	tEnter = fmaxf( tEnter, 0.f );

	result.m_didImpact = true;
	result.m_impactDist = tEnter * maxDist;
	result.m_impactPos = startPos + fwdNormal * result.m_impactDist;
	result.m_impactNormal = Vec3( hit2D.m_impactNormal.x, hit2D.m_impactNormal.y, 0.f );

	// Hit top or bottom
	bool hitCap = ( tEnter == tCylRange.m_min ) && ( tEnter > tSideRange.m_min );
	if ( hitCap )
	{
		float capNormalZ = ( fwdNormal.z < 0.f ) ? 1.f : -1.f;
		result.m_impactNormal = Vec3( 0.f, 0.f, capNormalZ );
	}

	return result;
}

//------------------------------------------------------------------------------
RaycastResult3D RaycastVsSphere( Vec3 startPos, Vec3 fwdNormal, float maxDist, Vec3 sphereCenter, float sphereRadius )
{
	Vec3 rayEndPoint = startPos + ( fwdNormal * maxDist );
	Vec3 rayToCenter = sphereCenter - startPos;
	Vec3 SCj = CrossProduct3D( rayToCenter, fwdNormal );

	RaycastResult3D result;
	if ( SCj.GetLength() >= sphereRadius )
	{
		return result; // Too far to the side
	}

	float SCi = DotProduct3D( rayToCenter, fwdNormal );
	if ( SCi >= maxDist + sphereRadius )
	{
		return result; // Too late; sphere is after ray end
	}

	if ( SCi <= -sphereRadius )
	{
		return result; // Too early; sphere is before ray start
	}

	float adjustmentDistance = sqrtf( ( sphereRadius * sphereRadius ) - ( SCj.GetLengthSquared() ) );
	float impactDist = SCi - adjustmentDistance;
	if ( impactDist >= maxDist || impactDist <= 0.f )
	{
		return result; // Impact point is outside of ray segment
	}

	result.m_didImpact = true;
	result.m_impactDist = impactDist;
	result.m_impactPos = startPos + ( fwdNormal * result.m_impactDist );
	result.m_impactNormal = ( result.m_impactPos - sphereCenter ).GetNormalized();
	return result;
}

//------------------------------------------------------------------------------
RaycastResult3D RaycastVsAABB3( Vec3 startPos, Vec3 fwdNormal, float maxDist, Vec3 boxMins, Vec3 boxMaxs )
{
	RaycastResult3D result;
	if ( IsPointInsideAABB3( startPos, boxMins, boxMaxs ) )
	{
		result.m_didImpact = true;
		result.m_impactDist = 0.f;
		result.m_impactPos = startPos;
		result.m_impactNormal = -fwdNormal;
		return result;
	}
	Vec3 rayEndPoint = startPos + ( fwdNormal * maxDist );
	Vec3 rayMins = Vec3( GetFloatMin( rayEndPoint.x, startPos.x ), GetFloatMin( rayEndPoint.y, startPos.y ), GetFloatMin( rayEndPoint.z, startPos.z ) );
	Vec3 rayMaxs = Vec3( GetFloatMax( rayEndPoint.x, startPos.x ), GetFloatMax( rayEndPoint.y, startPos.y ), GetFloatMax( rayEndPoint.z, startPos.z ) );
	if ( !DoAABB3sOverlap( rayMins, rayMaxs, boxMins, boxMaxs ) )
	{
		return result;
	}
	float xLength = rayEndPoint.x - startPos.x;
	float tMinX = ( boxMins.x - startPos.x ) / xLength;
	float tMaxX = ( boxMaxs.x - startPos.x ) / xLength;
	float yLength = rayEndPoint.y - startPos.y;
	float tMinY = ( boxMins.y - startPos.y ) / yLength;
	float tMaxY = ( boxMaxs.y - startPos.y ) / yLength;
	float zLength = rayEndPoint.z - startPos.z;
	float tMinZ = ( boxMins.z - startPos.z ) / zLength;
	float tMaxZ = ( boxMaxs.z - startPos.z ) / zLength;
	if ( tMinX > tMaxX )
	{
		float temp = tMinX;
		tMinX = tMaxX;
		tMaxX = temp;
	}
	if ( tMinY > tMaxY )
	{
		float temp = tMinY;
		tMinY = tMaxY;
		tMaxY = temp;
	}
	if ( tMinZ > tMaxZ )
	{
		float temp = tMinZ;
		tMinZ = tMaxZ;
		tMaxZ = temp;
	}
	FloatRange xRange = FloatRange( tMinX, tMaxX );
	FloatRange yRange = FloatRange( tMinY, tMaxY );
	FloatRange zRange = FloatRange( tMinZ, tMaxZ );
	if ( !xRange.IsOverlappingWith( yRange ) || !xRange.IsOverlappingWith( zRange ) || !yRange.IsOverlappingWith( zRange ) )
	{
		return result;
	}
	float tEnter = tMinX;
	if ( tMinY > tEnter )
	{
		tEnter = tMinY;
	}
	if ( tMinZ > tEnter )
	{
		tEnter = tMinZ;
	}
	float tExit = tMaxX;
	if ( tMaxY < tExit )
	{
		tExit = tMaxY;
	}
	if ( tMaxZ < tExit )
	{
		tExit = tMaxZ;
	}
	if ( tEnter > tExit || tEnter >= 1.f || tExit <= 0.f )
	{
		return result;
	}
	result.m_didImpact = true;
	result.m_impactDist = tEnter * maxDist;
	result.m_impactPos = startPos + ( fwdNormal * result.m_impactDist );
	if ( tMinX > tMinY && tMinX > tMinZ )
	{
		if ( fwdNormal.x < 0.f )
		{
			result.m_impactNormal = Vec3( 1.f, 0.f, 0.f );
		}
		else
		{
			result.m_impactNormal = Vec3( -1.f, 0.f, 0.f );
		}
	}
	else if ( tMinY > tMinZ )
	{
		if ( fwdNormal.y < 0.f )
		{
			result.m_impactNormal = Vec3( 0.f, 1.f, 0.f );
		}
		else
		{
			result.m_impactNormal = Vec3( 0.f, -1.f, 0.f );
		}
	}
	else
	{
		if ( fwdNormal.z < 0.f )
		{
			result.m_impactNormal = Vec3( 0.f, 0.f, 1.f );
		}
		else
		{
			result.m_impactNormal = Vec3( 0.f, 0.f, -1.f );
		}
	}
	return result;
}

//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnAABB2D( Vec2 referencePos, AABB2 const& alignedBox )
{
	if ( IsPointInsideAABB2D( referencePos, alignedBox ) )
	{
		return referencePos;
	}

	Vec2 aabb2Corners[4];
	aabb2Corners[0] = alignedBox.m_mins;                               
	aabb2Corners[1] = Vec2( alignedBox.m_maxs.x, alignedBox.m_mins.y );
	aabb2Corners[2] = alignedBox.m_maxs;                               
	aabb2Corners[3] = Vec2( alignedBox.m_mins.x, alignedBox.m_maxs.y );

	Vec2 nearestAB = GetNearestPointOnLineSegment2D( referencePos, aabb2Corners[0], aabb2Corners[1] );
	Vec2 nearestBC = GetNearestPointOnLineSegment2D( referencePos, aabb2Corners[1], aabb2Corners[2] );
	Vec2 nearestCD = GetNearestPointOnLineSegment2D( referencePos, aabb2Corners[2], aabb2Corners[3] );
	Vec2 nearestDA = GetNearestPointOnLineSegment2D( referencePos, aabb2Corners[3], aabb2Corners[0] );

	float distAB = GetDistanceSquared2D( nearestAB, referencePos );
	float distBC = GetDistanceSquared2D( nearestBC, referencePos );
	float distCD = GetDistanceSquared2D( nearestCD, referencePos );
	float distDA = GetDistanceSquared2D( nearestDA, referencePos );

	if ( distAB <= distBC && distAB <= distCD && distAB <= distDA )
	{
		return nearestAB;
	}
	if ( distBC <= distCD && distBC <= distDA )
	{
		return nearestBC;
	}
	if ( distCD <= distDA )
	{
		return nearestCD;
	}
	
	return nearestDA;
}

//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnOBB2D(Vec2 referencePos, OBB2 const& orientedBox)
{
	if (IsPointInsideOBB2D(referencePos, orientedBox))
	{
		return referencePos;
	}

	Vec2 obb2Corners[4];
	orientedBox.GetCornerPoints(obb2Corners);

	Vec2 nearestAB = GetNearestPointOnLineSegment2D(referencePos, obb2Corners[0], obb2Corners[1]);
	Vec2 nearestBC = GetNearestPointOnLineSegment2D(referencePos, obb2Corners[1], obb2Corners[2]);
	Vec2 nearestCD = GetNearestPointOnLineSegment2D(referencePos, obb2Corners[2], obb2Corners[3]);
	Vec2 nearestDA = GetNearestPointOnLineSegment2D(referencePos, obb2Corners[3], obb2Corners[0]);

	float distAB = GetDistanceSquared2D(nearestAB, referencePos);
	float distBC = GetDistanceSquared2D(nearestBC, referencePos);
	float distCD = GetDistanceSquared2D(nearestCD, referencePos);
	float distDA = GetDistanceSquared2D(nearestDA, referencePos);

	if (distAB <= distBC && distAB <= distCD && distAB <= distDA)
	{
		return nearestAB;
	}
	if (distBC <= distCD && distBC <= distDA)
	{
		return nearestBC;
	}
	 if (distCD <= distDA)
	{
		return nearestCD;
	}

	return nearestDA;
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

//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnCapsule2D( Vec2 referencePos, Vec2 boneStart, Vec2 boneEnd, float radius )
{
	if( IsPointInsideCapsule2D(referencePos, boneStart, boneEnd, radius) )
		return referencePos;

	Vec2 nearestOnBone = GetNearestPointOnLineSegment2D(referencePos, boneStart, boneEnd);

	Vec2 boneToPoint = referencePos - nearestOnBone;
	return nearestOnBone + ( boneToPoint.GetNormalized() * radius );
}

//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnTriangle2D( Vec2 referencePos, Vec2 ccw0, Vec2 ccw1, Vec2 ccw2 )
{
	if ( IsPointInsideTriangle2D( referencePos, ccw0, ccw1, ccw2 ) )
	{
		return referencePos;
	}

	Vec2 nearestAB = GetNearestPointOnLineSegment2D( referencePos, ccw0, ccw1);
	Vec2 nearestBC = GetNearestPointOnLineSegment2D( referencePos, ccw1, ccw2 );
	Vec2 nearestCA = GetNearestPointOnLineSegment2D( referencePos, ccw2, ccw0 );

	float distAB = GetDistanceSquared2D( nearestAB, referencePos );
	float distBC = GetDistanceSquared2D( nearestBC, referencePos );
	float distCA = GetDistanceSquared2D( nearestCA, referencePos );

	if (distAB <= distBC && distAB <= distCA)
	{
		return nearestAB;
	}
	if ( distBC <= distCA )
	{
		return nearestBC;
	}

	return nearestCA;
}

//------------------------------------------------------------------------------
Vec3 GetNearestPointOnSphere( Vec3 referencePos, Vec3 sphereCenter, float sphereRadius )
{
	Vec3 centerToPoint = referencePos - sphereCenter;
	return sphereCenter + ( centerToPoint.GetNormalized() * sphereRadius );
}

//------------------------------------------------------------------------------
Vec3 GetNearestPointOnCylinder( Vec3 referencePos, Vec3 cylinderStart, Vec3 cylinderEnd, float radius )
{
	FloatRange zRange( cylinderStart.z, cylinderEnd.z );
	FloatRange pointZRange( referencePos.z, referencePos.z );
	Vec2 pointXY = Vec2( referencePos.x, referencePos.y );
	Vec2 cylinderXY = Vec2( cylinderStart.x, cylinderStart.y );
	Vec2 nearestOnDisc = GetNearestPointOnDisc2D( pointXY, cylinderXY, radius );
	if ( zRange.IsOverlappingWith( pointZRange ) )
	{
		return Vec3( nearestOnDisc.x, nearestOnDisc.y, referencePos.z );
	}
	return Vec3( nearestOnDisc.x, nearestOnDisc.y, GetClamped( referencePos.z, cylinderStart.z, cylinderEnd.z ) );
}

//------------------------------------------------------------------------------
Vec3 GetNearestPointOnAABB3( Vec3 referencePos, Vec3 boxMins, Vec3 boxMaxs )
{
	float x = GetClamped( referencePos.x, boxMins.x, boxMaxs.x );
	float y = GetClamped( referencePos.y, boxMins.y, boxMaxs.y );
	float z = GetClamped( referencePos.z, boxMins.z, boxMaxs.z );
	return Vec3( x, y, z );
}

//------------------------------------------------------------------------------
float ComputeCubicBezier1D( float A, float B, float C, float D, float t )
{
	float AB = Interpolate( A, B, t );
	float BC = Interpolate( B, C, t );
	float CD = Interpolate( C, D, t );

	float ABC = Interpolate( AB, BC, t );
	float BCD = Interpolate( BC, CD, t );

	return Interpolate( ABC, BCD, t );
}

//------------------------------------------------------------------------------
float ComputeQuinticBezier1D( float A, float B, float C, float D, float E, float F, float t )
{
	float AB = Interpolate( A, B, t );
	float BC = Interpolate( B, C, t );
	float CD = Interpolate( C, D, t );
	float DE = Interpolate( D, E, t );
	float EF = Interpolate( E, F, t );

	float ABC = Interpolate( AB, BC, t );
	float BCD = Interpolate( BC, CD, t );
	float CDE = Interpolate( CD, DE, t );
	float DEF = Interpolate( DE, EF, t );

	float ABCD = Interpolate( ABC, BCD, t );
	float BCDE = Interpolate( BCD, CDE, t );
	float CDEF = Interpolate( CDE, DEF, t );

	float ABCDE = Interpolate( ABCD, BCDE, t );
	float BCDEF = Interpolate( BCDE, CDEF, t );

	return Interpolate( ABCDE, BCDEF, t );
}

//------------------------------------------------------------------------------
float SmoothStart2( float t )
{
	return t * t;
}

//------------------------------------------------------------------------------
float SmoothStart3( float t )
{
	return t * t * t;
}

//------------------------------------------------------------------------------
float SmoothStart4( float t )
{
	return t * t * t * t;
}

//------------------------------------------------------------------------------
float SmoothStart5( float t )
{
	return t * t * t * t * t;
}

//------------------------------------------------------------------------------
float SmoothStart6( float t )
{
	return t * t * t * t * t * t;
}

//------------------------------------------------------------------------------
float SmoothStop2( float t )
{
	float inv = 1.f - t;
	return 1.f - ( inv * inv );
}

//------------------------------------------------------------------------------
float SmoothStop3( float t )
{
	float inv = 1.f - t;
	return 1.f - ( inv * inv * inv );
}

//------------------------------------------------------------------------------
float SmoothStop4( float t )
{
	float inv = 1.f - t;
	return 1.f - ( inv * inv * inv * inv );
}

//------------------------------------------------------------------------------
float SmoothStop5( float t )
{
	float inv = 1.f - t;
	return 1.f - ( inv * inv * inv * inv * inv );
}

//------------------------------------------------------------------------------
float SmoothStop6( float t )
{
	float inv = 1.f - t;
	return 1.f - ( inv * inv * inv * inv * inv * inv );
}

//------------------------------------------------------------------------------
float SmoothStep3( float t )
{
	return Interpolate( SmoothStart2(t), SmoothStop2(t), t );
}

//------------------------------------------------------------------------------
float SmoothStep5( float t )
{
	return ComputeQuinticBezier1D( 0, 0, 0, 1, 1, 1, t );
}

//------------------------------------------------------------------------------
float Hesitate3( float t )
{
	return ComputeCubicBezier1D( 0, 1, 0, 1, t );
}

//------------------------------------------------------------------------------
float Hesitate5( float t )
{
	return ComputeQuinticBezier1D( 0, 1, 0, 1, 0, 1, t );
}

//------------------------------------------------------------------------------
float CustomFunkyEasingFunction( float t )
{
	return ComputeQuinticBezier1D( 0, 0, 1, 0, 1, 1, t ); // #todo make this something actually funky
}

//-----------------------------------------------------------------------------------------------
float GetFloatMax( float a, float b )
{
	if ( a > b )
	{
		return a;
	}
	return b;
}

//-----------------------------------------------------------------------------------------------
float GetFloatMin( float a, float b )
{
	if ( a < b )
	{
		return a;
	}
	return b;
}

//------------------------------------------------------------------------------
Mat44 GetBillboardTransform( BillboardType billboardType, Mat44 const& targetTransform, const Vec3& billboardPosition, const Vec2& billboardScale /*= Vec2( 1.0f, 1.0f ) */ )
{
	Mat44 result;
	Vec3 cameraPos = targetTransform.GetTranslation3D();
	Vec3 dirToCamera = cameraPos - billboardPosition;
	dirToCamera = dirToCamera.GetNormalized();

	if ( billboardType == BillboardType::WORLD_UP_FACING )
	{
		Vec3 iBasis = dirToCamera;
		Vec3 kBasis = Vec3::Z_AXIS;
		Vec3 jBasis = CrossProduct3D( kBasis, iBasis ).GetNormalized();
		result.SetIJKT3D( iBasis, jBasis, kBasis, billboardPosition );
		result.AppendScaleNonUniform3D( Vec3( billboardScale.x, billboardScale.y, 1.f ) );
	}

	if ( billboardType == BillboardType::WORLD_UP_OPPOSING )
	{
		Vec3 iBasis = -dirToCamera;
		Vec3 kBasis = Vec3::Z_AXIS;
		Vec3 jBasis = CrossProduct3D( kBasis, iBasis ).GetNormalized();
		result.SetIJKT3D( iBasis, jBasis, kBasis, billboardPosition );
		result.AppendScaleNonUniform3D( Vec3( billboardScale.x, billboardScale.y, 1.f ) );
	}

	if ( billboardType == BillboardType::FULL_FACING )
	{
		Vec3 iBasis = dirToCamera;
		Vec3 jBasis = CrossProduct3D( Vec3::Z_AXIS, iBasis ).GetNormalized();
		Vec3 kBasis = CrossProduct3D( iBasis, jBasis ).GetNormalized();
		result.SetIJKT3D( iBasis, jBasis, kBasis, billboardPosition );
		result.AppendScaleNonUniform3D( Vec3( billboardScale.x, billboardScale.y, 1.f ) );
	}

	if ( billboardType == BillboardType::FULL_OPPOSING )
	{
		Vec3 iBasis = targetTransform.GetIBasis3D();
		Vec3 jBasis = targetTransform.GetJBasis3D();
		Vec3 kBasis = targetTransform.GetKBasis3D();
		result.SetIJKT3D( -iBasis, -jBasis, kBasis, billboardPosition );
		result.AppendScaleNonUniform3D( Vec3( billboardScale.x, billboardScale.y, 1.f ) );
	}

	return result;
}

//-----------------------------------------------------------------------------------------------
Vec3 WorldToFMOD( const Vec3 worldPos )
{
	Vec3 fmodPos;
	fmodPos.x = -worldPos.y;
	fmodPos.y = worldPos.z;
	fmodPos.z = -worldPos.x;
	
	return fmodPos;
}

