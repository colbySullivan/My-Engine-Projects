#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/AABB3.hpp"
#define _USE_MATH_DEFINES // Used for PI

//-----------------------------------------------------------------------------------------------
struct RaycastResult2D
{
	// Basic raycast result information (required)
	bool	m_didImpact = false;
	float	m_impactDist = 0.f;
	Vec2	m_impactPos;
	Vec2	m_impactNormal;
};

//-----------------------------------------------------------------------------------
// Forward type declarations
struct Vec2;
struct Vec3;
struct Vec4;
struct Mat44;

//-----------------------------------------------------------------------------------
// Clamp and Lerp
float   GetClamped(float value, float minValue, float maxValue);
float   GetClampedZeroToOne(float value);
float   Interpolate(float start, float end, float fractionTowardEnd);
float	InterpolateShortestDis(float start, float end, float fractionTowardEnd);
float   GetFractionWithinRange(float value, float rangeStart, float rangeEnd);
float   RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd);
float   RangeMapClamped(float inValue, float inStart, float inEnd, float outStart, float outEnd);
int     RoundDownToInt(float value);
float	NormalizeByte( unsigned char byte );
unsigned char DenormalizeByte( float value );


//-----------------------------------------------------------------------------------
// Angle utilities
float   ConvertDegreesToRadians(float degrees);
float   ConvertRadiansToDegrees(float radians);
float   CosDegrees(float degrees);
float   SinDegrees(float degrees);
float   Atan2Degrees(float y, float x);
float   GetShortestAngularDispDegrees( float startDegrees, float endDegrees );
float	GetTurnedTowardDegrees( float currentDegrees, float goalDegrees, float maxDeltaDegrees );
float	GetAngleDegreesBetweenVectors2D( Vec2 const& a, Vec2 const& b );

//-----------------------------------------------------------------------------------
// Dots and Cross
float	DotProduct2D( Vec2 const& a, Vec2 const& b );
float	DotProduct3D( Vec3 const& a, Vec3 const& b );
float	DotProduct4D( Vec4 const& a, Vec4 const& b );
float	CrossProduct2D( Vec2 const& a, Vec2 const& b );
Vec3	CrossProduct3D( Vec3 const& a, Vec3 const& b );	

//-----------------------------------------------------------------------------------
// Basic 2D & 3D utilities
float   GetDistance2D(Vec2 const& positionA, Vec2 const& positionB);
float   GetDistanceSquared2D(Vec2 const& positionA, Vec2 const& positionB);
float   GetDistance3D(Vec3 const& positionA, Vec3 const& positionB);
float   GetDistanceSquared3D(Vec3 const& positionA, Vec3 const& positionB);
float   GetDistanceXY3D(Vec3 const& positionA, Vec3 const& positionB);
float   GetDistanceXYSquared3D(Vec3 const& positionA, Vec3 const& positionB);
int		GetTaxicabDistance2D( IntVec2 const& pointA, IntVec2 const& pointB );
float	GetProjectedLength2D( Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto ); // works if Vecs not normalized
Vec2	GetProjectedVector2D( Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto ); // works if Vecs not normalized

//-----------------------------------------------------------------------------------
// Geometric query utilities
bool    DoDiscsOverlap(Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB);
bool    DoSpheresOverlap(Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB);
bool    DoAABB2sOverlap( AABB2 const& alignedBoxA, AABB2 const& alignedBoxB );
bool	DoCylindersOverlap( const Vec3& startA, const Vec3& endA, float radiusA, const Vec3& startB, const Vec3& endB, float radiusB );
bool	DoSphereABB3Overlap( Vec3 const& sphereCenter, float sphereRadius, Vec3 const& boxMins, Vec3 const& boxMaxs );
bool	DoSphereCylinderOverlap( Vec3 const& sphereCenter, float sphereRadius, const Vec3& start, const Vec3& end, float radius );
bool	IsPointInsideDisc2D( Vec2 const& point, Vec2 const& discCenter, float discRadius );
bool	IsPointInsideOrientedSector2D( Vec2 const& point, Vec2 const& sectorTip, float sectorFwdDegrees, float sectorApertureDegrees, float sectorRadius );
bool	IsPointInsideDirectedSector2D( Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorFwdNormal, float sectorApertureDegrees, float sectorRadius );
Vec2	GetNearestPointOnDisc2D( Vec2 const& referencePos, Vec2 const& discCenter, float discRadius );
Vec3    GetNearestPointOnAABB3D( Vec3 const& referencePos, const AABB3& bounds );
bool	PushDiscOutOfFixedPoint2D( Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedPoint );
bool	PushDiscOutOfFixedDisc2D( Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius );
bool	PushDiscsOutOfEachOther2D( Vec2& aCenter, float aRadius, Vec2& bCenter, float bRadius );
bool	PushDiscOutOfFixedAABB2D( Vec2& mobileDiscCenter, float discRadius, AABB2 const& fixedBox );
bool	PushCylinderOutOfFixedCylinder( Vec3& mobileStart, Vec3& mobileEnd, float mobileRadius, const Vec3& fixedStart, const Vec3& fixedEnd, float fixedRadius );

//-----------------------------------------------------------------------------------
// Transform utilities
void    TransformPosition2D(Vec2& posToTransform, float uniformScale, float rotationDegrees, Vec2 const& translation);
void    TransformPositionXY3D(Vec3& posToTransform, float xyScale, float zRotationDegrees, Vec2 const& xyTranslation);
void	TransformPosition2D( Vec2& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation );
void	TransformPositionXY3D( Vec3& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation );

//-----------------------------------------------------------------------------------------------
bool    IsPointInsideAABB2D(Vec2 point, AABB2 const& alignedBox);
bool    IsPointInsideOBB2D(Vec2 point, OBB2 const& orientedBox);
bool    IsPointInsideCapsule2D(Vec2 point, Vec2 boneStart, Vec2 boneEnd, float radius);
bool    IsPointInsideTriangle2D(Vec2 point, Vec2 ccw0, Vec2 ccw1, Vec2 ccw2); // Counter-Clockwise (positive winding)
RaycastResult2D RaycastVsDisc2D( Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 discCenter, float discRadius );
RaycastResult2D RaycastVsLine2D( Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 pointA, Vec2 pointB );
RaycastResult2D RaycastVsAABB22D( Vec2 startPos, Vec2 fwdNormal, float maxDist, AABB2& box );

//-----------------------------------------------------------------------------------------------
Vec2    GetNearestPointOnAABB2D(Vec2 referencePos, AABB2 const& alignedBox);
Vec2    GetNearestPointOnOBB2D(Vec2 referencePos, OBB2 const& orientedBox);
Vec2    GetNearestPointOnInfiniteLine2D(Vec2 referencePos, Vec2 pointOnLine, Vec2 anotherPointOnLine);
Vec2    GetNearestPointOnLineSegment2D(Vec2 referencePos, Vec2 start, Vec2 end);
Vec2    GetNearestPointOnCapsule2D(Vec2 referencePos, Vec2 boneStart, Vec2 boneEnd, float radius);
Vec2    GetNearestPointOnTriangle2D(Vec2 referencePos, Vec2 ccw0, Vec2 ccw1, Vec2 ccw2); // Counter-Clockwise (positive winding)

//-----------------------------------------------------------------------------------------------
// Basic util functions
float	GetFloatMax( float a, float b );
float	GetFloatMin( float a, float b );

//-----------------------------------------------------------------------------------------------
// Billboard functions
enum class BillboardType
{
	NONE = -1,
	WORLD_UP_FACING,
	WORLD_UP_OPPOSING,
	FULL_FACING,
	FULL_OPPOSING,
	COUNT
};

Mat44 GetBillboardTransform( BillboardType billboardType, Mat44 const& targetTransform, const Vec3& billboardPosition, const Vec2& billboardScale = Vec2( 1.0f, 1.0f ) );