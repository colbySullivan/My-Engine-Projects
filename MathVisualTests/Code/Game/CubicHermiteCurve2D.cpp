#include "Game/CubicHermiteCurve2D.hpp"
#include "Game/CubicBezierCurve2D.hpp"
#include "Engine/Math/MathUtils.hpp"

//------------------------------------------------------------------------------
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
	m_endPos = 3.f * ( fromBezier.GetEndPos() - fromBezier.GetGuidePos2() );
	m_endPos = fromBezier.GetEndPos();
}
