#include "Camera.hpp"

void Camera::SetOrthoView(Vec2 const& bottomLeft, Vec2 const& topRight)
{
	m_orthoBottomLeft = bottomLeft;
	m_orthoTopRight = topRight;
}

Vec2 Camera::GetOrthoBottomLeft() const
{
	return Vec2(m_orthoBottomLeft.x, m_orthoBottomLeft.y );
}

Vec2 Camera::GetOrthoTopRight() const
{
	return Vec2(m_orthoTopRight.x, m_orthoTopRight.y);
}
