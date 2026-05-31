#include "Engine/Math/Plane3D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/FloatRange.hpp"

//-----------------------------------------------------------------------------------------------
Plane3D::Plane3D( Vec3 normal, Vec3 centerPoint )
{
	m_normal = normal.GetNormalized();
	m_dist = DotProduct3D( centerPoint, m_normal );
}

//-----------------------------------------------------------------------------------------------
Vec3 Plane3D::GetNormal()
{
	return m_normal;
}

//-----------------------------------------------------------------------------------------------
float Plane3D::GetPlaneDistance()
{
	return m_dist;
}

//-----------------------------------------------------------------------------------------------
float Plane3D::GetAltitude( Vec3 point )
{
	return DotProduct3D( point, m_normal ) - m_dist;
}

//-----------------------------------------------------------------------------------------------
bool Plane3D::isFrontSide( Vec3 point )
{
	return DotProduct3D( point, m_normal ) > m_dist;
}

//-----------------------------------------------------------------------------------------------
bool Plane3D::isBackSide( Vec3 point )
{
	return DotProduct3D( point, m_normal ) < m_dist;
}

//-----------------------------------------------------------------------------------------------
bool Plane3D::isPointOnPlane( Vec3 point )
{
	float tolerance = 0.001;
	float altitude = GetAltitude( point );
	FloatRange toleranceRange = FloatRange( -tolerance, tolerance );
	if ( toleranceRange.IsOnRange( altitude ) )
	{
		return true;
	}

	return false;
}