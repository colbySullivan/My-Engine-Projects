#include "Camera.hpp"

//-----------------------------------------------------------------------------------------------
void Camera::SetOrthographicView( Vec2 const& bottomLeft, Vec2 const& topRight, float near /*= 0.0f*/, float far /*= 1.0f */ )
{
	m_orthographicBottomLeft = bottomLeft;
	m_orthographicTopRight = topRight;
	m_orthographicNear = near;
	m_orthographicFar = far;
}

//-----------------------------------------------------------------------------------------------
void Camera::SetPerspectiveView( float aspect, float fov, float near, float far )
{
	m_perspectiveAspect = aspect;
	m_perspectiveFOV = fov;
	m_perspectiveNear = near;
	m_perspectiveFar = far;
}

//-----------------------------------------------------------------------------------------------
Mat44 Camera::GetCameraToWorldTransform() const
{
	Mat44 cameraToWorld;
	Mat44 orientationRotation = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
	cameraToWorld.AppendTranslation3D(m_position);
	cameraToWorld.Append(orientationRotation);

	return cameraToWorld;
}

//-----------------------------------------------------------------------------------------------
Mat44 Camera::GetWorldToCameraTransform() const
{
	return GetCameraToWorldTransform().GetOrthonormalInverse();
}

//-----------------------------------------------------------------------------------------------
Mat44 Camera::GetOrthographicMatrix() const
{
	return Mat44::MakeOrthoProjection( m_orthographicBottomLeft.x, m_orthographicTopRight.x, m_orthographicBottomLeft.y, m_orthographicTopRight.y, m_orthographicNear, m_orthographicFar );
}

//-----------------------------------------------------------------------------------------------
Mat44 Camera::GetPerspectiveMatrix() const
{
	return Mat44::MakePerspectiveProjection( m_perspectiveFOV, m_perspectiveAspect, m_perspectiveNear, m_perspectiveFar );
}

//-----------------------------------------------------------------------------------------------
Mat44 Camera::GetProjectionMatrix() const
{
	if ( m_mode == eMode_Orthographic )
	{
		return GetOrthographicMatrix();
	}
	if ( m_mode == eMode_Perspective )
	{
		return GetPerspectiveMatrix();
	}
	return GetPerspectiveMatrix();
}
