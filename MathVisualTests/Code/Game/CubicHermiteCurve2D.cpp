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