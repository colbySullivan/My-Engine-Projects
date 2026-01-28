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
	value = GetClampedZeroToOne(value);
	int byteValue = static_cast<int>(value * 256.f);
	if (byteValue > 255)
	{
		byteValue = 255;
	}

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

//------------------------------------------------------------------------------
float CrossProduct2D(Vec2 const& a, Vec2 const& b)
{
	return 0.f; // TODO
}

//------------------------------------------------------------------------------
Vec3 CrossProduct3D(Vec3 const& a, Vec3 const& b)
{
	return Vec3(0.f,0.f,0.f); // TODO
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

	if ( IsPointInsideDisc2D( startPos, discCenter, discRadius ) )
	{
		// Hit started inside the disc
		result.m_didImpact = true;
		result.m_impactDist = 0.f;
		result.m_impactPos = startPos;
		result.m_impactNormal = -fwdNormal;
		return result;
	}	

	float adjustmentDistance = sqrtf( ( discRadius * discRadius ) - ( SCj * SCj ) );
	float impactDist = SCi - adjustmentDistance;

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

