#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Game/CubicHermiteCurve2D.hpp"

//-----------------------------------------------------------------------------------------------
class CubicBezierCurve2D
{
public:
	CubicBezierCurve2D( Vec2 startPos, Vec2 guidePos1, Vec2 guidePos2, Vec2 endPos );
	explicit CubicBezierCurve2D( CubicHermiteCurve2D const& fromHermite );
	Vec2 EvaluateAtParametric( float parametricZeroToOne ) const;
	float GetApproximateLength( int numSubdivisions = 64 ) const;
	Vec2 EvaluateAtApproximateDistance( float distanceAlongCurve, int numSubdivisions = 64 ) const;

	Vec2 GetStartPos() const { return m_startPos; }
	Vec2 GetGuidePos1() const { return m_guidePos1; }
	Vec2 GetGuidePos2() const { return m_guidePos2; }
	Vec2 GetEndPos() const { return m_endPos; }

private:
	Vec2 m_startPos;
	Vec2 m_guidePos1;
	Vec2 m_guidePos2;
	Vec2 m_endPos;
};