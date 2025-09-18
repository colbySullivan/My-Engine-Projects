#include "Engine/Math/AABB2.hpp"

AABB2::AABB2(AABB2 const& copyFrom)
	: m_mins(copyFrom.m_mins)
	, m_maxs(copyFrom.m_maxs)
{

}

AABB2::AABB2(float minX, float minY, float maxX, float maxY)
{
	m_mins = Vec2(minX,minY);
	m_maxs = Vec2(maxX,maxY);
}

AABB2::AABB2(Vec2 const& mins, Vec2 const& maxs)
{
	m_mins = mins;
	m_maxs = maxs;
}

bool AABB2::IsPointInside(Vec2 const& point) const
{
	return true; // TODO
}

Vec2 const AABB2::GetCenter() const
{
	float xCenter = (m_mins.x + m_maxs.x) * 0.5;
	float yCenter = (m_mins.y + m_maxs.y) * 0.5;
	return Vec2(xCenter,yCenter);
}

Vec2 const AABB2::GetDimensions() const
{
	float xLength = m_maxs.x - m_mins.x;
	float yLength = m_maxs.y - m_mins.y;
	return Vec2(xLength, yLength);
}

Vec2 const AABB2::GetNearestPoint(Vec2 const& referencePosition) const
{
	return referencePosition; // TODO
}

Vec2 const AABB2::GetPointAtUV(Vec2 const& uv) const
{
	return uv; // TODO
}

Vec2 const AABB2::GetUVForPoint(Vec2 const& point) const
{
	return point; // TODO
}

void AABB2::Translate(Vec2 const& translationToApply)
{

}

void AABB2::SetCenter(Vec2 const& newCenter)
{

}

void AABB2::SetDimensions(Vec2 const& newDimensions)
{

}

void AABB2::StretchToIncludePoint(Vec2 const& point)
{

}

