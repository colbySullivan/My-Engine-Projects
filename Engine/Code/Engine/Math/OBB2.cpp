#include "Engine/Math/OBB2.hpp"
#include "MathUtils.hpp"

//------------------------------------------------------------------------------
OBB2::OBB2(Vec2 const& center, Vec2 const& iBasisNormal, Vec2 const& halfDimensions)
	: m_center(center)
	, m_iBasisNormal(iBasisNormal)
	, m_halfDimensions(halfDimensions)
{

}

Vec2 OBB2::GetLocalPosForWorldPos(Vec2 const& worldPos) const
{
	Vec2 jBasisNormal = Vec2(-m_iBasisNormal.y, m_iBasisNormal.x);

	Vec2 displacement = worldPos - m_center;

	float localX = DotProduct2D(displacement, m_iBasisNormal);
	float localY = DotProduct2D(displacement, jBasisNormal);

	return Vec2(localX, localY);
}

Vec2 OBB2::GetWorldPosForLocalPos(Vec2 const& localPos) const
{
	Vec2 jBasisNormal = Vec2(-m_iBasisNormal.y, m_iBasisNormal.x);

	Vec2 iOffset = m_iBasisNormal * localPos.x;
	Vec2 jOffset = jBasisNormal * localPos.y;

	Vec2 worldPos = m_center + iOffset + jOffset;

	return worldPos;
}

