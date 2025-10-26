#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/OBB2.hpp"
#define _USE_MATH_DEFINES // Used for PI

//-----------------------------------------------------------------------------------
// Forward type declarations
struct Vec2;
struct Vec3;

//-----------------------------------------------------------------------------------
// Clamp and Lerp
float   GetClamped(float value, float minValue, float maxValue);
float   GetClampedZeroToOne(float value);
float   Interpolate(float start, float end, float fractionTowardEnd);
float   GetFractionWithinRange(float value, float rangeStart, float rangeEnd);
float   RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd);
float   RangeMapClamped(float inValue, float inStart, float inEnd, float outStart, float outEnd);
int     RoundDownToInt(float value);

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
bool	IsPointInsideDisc2D( Vec2 const& point, Vec2 const& discCenter, float discRadius );
bool	IsPointInsideOrientedSector2D( Vec2 const& point, Vec2 const& sectorTip, float sectorFwdDegrees, float sectorApertureDegrees, float sectorRadius );
bool	IsPointInsideDirectedSector2D( Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorFwdNormal, float sectorApertureDegrees, float sectorRadius );
Vec2	GetNearestPointOnDisc2D( Vec2 const& referencePos, Vec2 const& discCenter, float discRadius );
bool	PushDiscOutOfFixedPoint2D( Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedPoint );
bool	PushDiscOutOfFixedDisc2D( Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius );
bool	PushDiscsOutOfEachOther2D( Vec2& aCenter, float aRadius, Vec2& bCenter, float bRadius );
bool	PushDiscOutOfFixedAABB2D( Vec2& mobileDiscCenter, float discRadius, AABB2 const& fixedBox );

//-----------------------------------------------------------------------------------
// Transform utilities
void    TransformPosition2D(Vec2& posToTransform, float uniformScale, float rotationDegrees, Vec2 const& translation);
void    TransformPositionXY3D(Vec3& posToTransform, float xyScale, float zRotationDegrees, Vec2 const& xyTranslation);
void	TransformPosition2D( Vec2& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation );
void	TransformPositionXY3D( Vec3& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation );

//-----------------------------------------------------------------------------------------------
// Is point inside? (Commented-out functions optional, but recommended if you have those classes
//bool    IsPointInsideDisc2D(Vec2 point, Vec2 discCenter, float discRadius);
//bool  IsPointInsideDisc2D( Vec2 point, Disc2 disc );
bool    IsPointInsideAABB2D(Vec2 point, AABB2 const& alignedBox);
bool    IsPointInsideOBB2D(Vec2 point, OBB2 const& orientedBox);
bool    IsPointInsideCapsule2D(Vec2 point, Vec2 boneStart, Vec2 boneEnd, float radius);
//bool  IsPointInsideCapsule2D( Vec2 point, Capsule2 const& capsule );
bool    IsPointInsideTriangle2D(Vec2 point, Vec2 ccw0, Vec2 ccw1, Vec2 ccw2); // Counter-Clockwise (positive winding)
//bool  IsPointInsideTriangle2D( Vec2 point, Triangle2D const& triangle );
bool    IsPointInsideOrientedSector2D(Vec2 point, Vec2 sectorTip, float sectorFwdDegrees, float sectorApertureDegrees, float sectorRadius);
bool    IsPointInsideDirectedSector2D(Vec2 point, Vec2 sectorTip, Vec2 sectorFwdNormal, float sectorApertureDegrees, float sectorRadius);

//-----------------------------------------------------------------------------------------------
// Get nearest point on...
Vec2    GetNearestPointOnDisc2D(Vec2 referencePos, Vec2 discCenter, float discRadius);
//Vec2  GetNearestPointOnDisc2D( Vec2 referencePos, Disc2 const& disc );
Vec2    GetNearestPointOnAABB2D(Vec2 referencePos, AABB2 const& alignedBox);
Vec2    GetNearestPointOnOBB2D(Vec2 referencePos, OBB2 const& orientedBox);
Vec2    GetNearestPointOnInfiniteLine2D(Vec2 referencePos, Vec2 pointOnLine, Vec2 anotherPointOnLine);
//Vec2  GetNearestPointOnInfiniteLine2D( Vec2 referencePos, LineSegment2 const& lineSegmentOnInfiniteLine );
Vec2    GetNearestPointOnLineSegment2D(Vec2 referencePos, Vec2 start, Vec2 end);
//Vec2  GetNearestPointOnLineSegment2D( Vec2 referencePos, LineSegment2 const& lineSegment );
Vec2    GetNearestPointOnCapsule2D(Vec2 referencePos, Vec2 boneStart, Vec2 boneEnd, float radius);
//Vec2  GetNearestPointOnCapsule2D( Vec2 referencePos, Capsule2 const& capsule );
Vec2    GetNearestPointOnTriangle2D(Vec2 referencePos, Vec2 ccw0, Vec2 ccw1, Vec2 ccw2); // Counter-Clockwise (positive winding)
//Vec2  GetNearestPointOnTriangle2D( Vec2 referencePos, Triangle2 const& triangle );

//-----------------------------------------------------------------------------------------------
// Corrective physics (push shapes out of discs, AABB2s, OBB2s, capsules, etc.)
bool    PushDiscOutOfFixedPoint2D(Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedPoint);
//bool  PushDiscOutOfFixedDisc2D( Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius );