#include "Engine/Math/Plane2D.hpp"
#include "Engine/Math/MathUtils.hpp"


Plane2D::Plane2D( Vec2 normal, Vec2 point )
{
	m_normal = normal.GetNormalized();
	m_dist = DotProduct2D( point, m_normal );
}

//-----------------------------------------------------------------------------------------------
Vec2 Plane2D::GetNormal()
{
	return m_normal;
}

//-----------------------------------------------------------------------------------------------
float Plane2D::GetPlaneDistance()
{
	return m_dist;
}

//-----------------------------------------------------------------------------------------------
float Plane2D::GetAltitude( Vec2 point )
{
	// Plane to the point // Negative means to the left and positive means to the right of the plane
	return DotProduct2D( point, m_normal ) - m_dist;
}

//-----------------------------------------------------------------------------------------------
bool Plane2D::isFrontSide( Vec2 point )
{
	return DotProduct2D( point, m_normal ) > m_dist;
}

//-----------------------------------------------------------------------------------------------
bool Plane2D::isBackSide( Vec2 point )
{
	return DotProduct2D( point, m_normal ) < m_dist;
}

//-----------------------------------------------------------------------------------------------
bool Plane2D::isOnPlane( Vec2 point )
{
	return !isFrontSide( point ) && !isBackSide( point );
}


