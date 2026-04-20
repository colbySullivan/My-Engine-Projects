#include "Game/CubicHermiteCurve2D.hpp"
#include "Game/CubicBezierCurve2D.hpp"
#include "Engine/Math/MathUtils.hpp"

//-----------------------------------------------------------------------------------------------
CubicHermiteCurve2D::CubicHermiteCurve2D( Vec2 startPos, Vec2 startVelocity, Vec2 endPos, Vec2 endVelocity )
	: m_startPos( startPos )
	, m_startVelocity( startVelocity )
	, m_endPos( endPos )
	, m_endVelocity( endVelocity )
{
}

//-----------------------------------------------------------------------------------------------
CubicHermiteCurve2D::CubicHermiteCurve2D( CubicBezierCurve2D const& fromBezier )
{
	m_startPos = fromBezier.GetStartPos();
	m_startVelocity = 3.f * ( fromBezier.GetGuidePos1() - fromBezier.GetStartPos() );
	m_endVelocity = 3.f * ( fromBezier.GetEndPos() - fromBezier.GetGuidePos2() );
	m_endPos = fromBezier.GetEndPos();
}

//-----------------------------------------------------------------------------------------------
Vec2 CubicHermiteCurve2D::EvaluateAtParametric( float parametricZeroToOne ) const
{
	CubicBezierCurve2D toBezier = CubicBezierCurve2D( *this );

	Vec2 startPos = toBezier.GetStartPos();
	Vec2 guidePost1 = toBezier.GetGuidePos1();
	Vec2 guidePost2 = toBezier.GetGuidePos2();
	Vec2 endPos = toBezier.GetEndPos();

	float t = GetClampedZeroToOne( parametricZeroToOne );
	float x = ComputeCubicBezier1D( startPos.x, guidePost1.x, guidePost2.x, endPos.x, t );
	float y = ComputeCubicBezier1D( startPos.y, guidePost1.y, guidePost2.y, endPos.y, t );
	return Vec2( x, y );
}

//-----------------------------------------------------------------------------------------------
float CubicHermiteCurve2D::GetApproximateLength( int numSubdivisions /*= 64 */ ) const
{
	float totalLength = 0.f;
	Vec2 previousPoint = m_startPos;

	for ( int i = 1; i <= numSubdivisions; ++i )
	{
		float t = static_cast< float >( i ) / static_cast< float >( numSubdivisions );
		Vec2 currentPoint = EvaluateAtParametric( t );
		totalLength += GetDistance2D( previousPoint, currentPoint );
		previousPoint = currentPoint;
	}

	return totalLength;
}

//------------------------------------------------------------------------------
Vec2 CubicHermiteCurve2D::EvaluateAtApproximateDistance( float distanceAlongCurve, int numSubdivisions /*= 64 */ ) const
{
	if ( distanceAlongCurve <= 0.f )
	{
		return m_startPos;
	}

	float accumulatedDistance = 0.f;
	Vec2 previousPoint = m_startPos;

	for ( int i = 1; i <= numSubdivisions; ++i )
	{
		float t = static_cast< float >( i ) / static_cast< float >( numSubdivisions );
		Vec2 currentPoint = EvaluateAtParametric( t );
		float segmentLength = GetDistance2D( previousPoint, currentPoint );

		if ( accumulatedDistance + segmentLength >= distanceAlongCurve )
		{
			float distanceIntoSegment = distanceAlongCurve - accumulatedDistance;
			float fractionIntoSegment = distanceIntoSegment / segmentLength;

			float x = Interpolate( previousPoint.x, currentPoint.x, fractionIntoSegment );
			float y = Interpolate( previousPoint.y, currentPoint.y, fractionIntoSegment );
			return Vec2( x, y );
		}

		accumulatedDistance += segmentLength;
		previousPoint = currentPoint;
	}

	return m_endPos;
}