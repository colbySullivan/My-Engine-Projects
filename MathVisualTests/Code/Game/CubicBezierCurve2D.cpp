#include "Game/CubicBezierCurve2D.hpp"
#include "Engine/Math/MathUtils.hpp"

//------------------------------------------------------------------------------
CubicBezierCurve2D::CubicBezierCurve2D( Vec2 startPos, Vec2 guidePos1, Vec2 guidePos2, Vec2 endPos )
	: m_startPos( startPos )
	, m_guidePos1( guidePos1 )
	, m_guidePos2( guidePos2 )
	, m_endPos( endPos )
{
	
}
//------------------------------------------------------------------------------
//CubicBezierCurve2D::CubicBezierCurve2D( CubicHermiteCurve2D const& fromHermite )
//{
//
//}

//------------------------------------------------------------------------------
Vec2 CubicBezierCurve2D::EvaluateAtParametric( float parametricZeroToOne ) const
{
	float t = GetClampedZeroToOne( parametricZeroToOne );
	float x = ComputeCubicBezier1D( m_startPos.x, m_guidePos1.x, m_guidePos2.x, m_endPos.x, t );
	float y = ComputeCubicBezier1D( m_startPos.y, m_guidePos1.y, m_guidePos2.y, m_endPos.y, t );
	return Vec2( x, y );
}

//------------------------------------------------------------------------------
float CubicBezierCurve2D::GetApproximateLength( int numSubdivisions /*= 64 */ ) const
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
Vec2 CubicBezierCurve2D::EvaluateAtApproximateDistance( float distanceAlongCurve, int numSubdivisions /*= 64 */ ) const
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

