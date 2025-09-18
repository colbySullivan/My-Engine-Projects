#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"

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
	return (point.x > m_mins.x && point.x < m_maxs.x && point.y > m_mins.y && point.y < m_maxs.y);
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
	float xNearest = referencePosition.x;
	float yNearest = referencePosition.y;
	if (IsPointInside(referencePosition))
		return referencePosition;
	if (referencePosition.x < m_mins.x)
		xNearest = m_mins.x;
	if (referencePosition.x > m_maxs.x)
		xNearest = m_maxs.x;
	if (referencePosition.y < m_mins.y)
		yNearest = m_mins.y;
	if (referencePosition.y > m_maxs.y)
		yNearest = m_maxs.y;
	
	return Vec2(xNearest, yNearest);
}

Vec2 const AABB2::GetPointAtUV(Vec2 const& uv) const
{
	return Vec2(Interpolate(m_mins.x, m_maxs.x, uv.x), Interpolate(m_mins.y, m_maxs.y, uv.y));
}

Vec2 const AABB2::GetUVForPoint(Vec2 const& point) const
{
	/*float xFraction = GetFractionWithinRange(point.x, m_mins.x, m_maxs.x);
	float yFraction = GetFractionWithinRange(point.y, m_mins.y, m_maxs.y);*/
	// TODO one case is incorrect
	return Vec2(RangeMapClamped(point.x, m_mins.x, m_maxs.x, 0.f, 1.f), RangeMapClamped(point.y, m_mins.y, m_maxs.y, 0.f, 1.f));
}

void AABB2::Translate(Vec2 const& translationToApply)
{

}

void AABB2::SetCenter(Vec2 const& newCenter)
{
	Vec2 currentDimensions = GetDimensions();
	float xOffset = currentDimensions.x * 0.5;
	float yOffset = currentDimensions.y * 0.5;
	m_mins.x = newCenter.x - xOffset;
	m_mins.y = newCenter.y - yOffset;
	m_maxs.x = xOffset + newCenter.x;
	m_maxs.y = yOffset + newCenter.y;
}

void AABB2::SetDimensions(Vec2 const& newDimensions)
{
	Vec2 centerBuffer = GetCenter();
	float xOffset = newDimensions.x * 0.5;
	float yOffset = newDimensions.y * 0.5;
	m_mins.x = centerBuffer.x - xOffset;
	m_mins.y = centerBuffer.y - yOffset;
	m_maxs.x = xOffset + centerBuffer.x;
	m_maxs.y = yOffset + centerBuffer.y;
}

void AABB2::StretchToIncludePoint(Vec2 const& point)
{
	if (!IsPointInside(point))
	{
		if (point.x <= m_mins.x)
			m_mins.x = point.x;
		if (point.x >= m_maxs.x)
			m_maxs.x = point.x;
		if (point.y <= m_mins.y)
			m_mins.y = point.x;
		if (point.y >= m_maxs.y)
			m_maxs.y = point.x;
	}
}

