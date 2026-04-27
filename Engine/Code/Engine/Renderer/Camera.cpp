#include "Camera.hpp"

//-----------------------------------------------------------------------------------------------
void Camera::SetOrthographicView( Vec2 const& bottomLeft, Vec2 const& topRight, float near /*= 0.0f*/, float far /*= 1.0f */ )
{
	m_mode = eMode_Orthographic;
	m_orthographicBottomLeft = bottomLeft;
	m_orthographicTopRight = topRight;
	m_orthographicNear = near;
	m_orthographicFar = far;
}

//-----------------------------------------------------------------------------------------------
void Camera::SetPerspectiveView( float aspect, float fov, float near, float far )
{
	m_mode = eMode_Perspective;
	m_perspectiveAspect = aspect;
	m_perspectiveFOV = fov;
	m_perspectiveNear = near;
	m_perspectiveFar = far;
}

//-----------------------------------------------------------------------------------------------
void Camera::SetPositionAndOrientation( const Vec3& position, const EulerAngles& orientation )
{
	m_position = position;
	m_orientation = orientation;
}

//-----------------------------------------------------------------------------------------------
void Camera::SetPosition( const Vec3& position )
{
	m_position = position;
}

Vec3 Camera::GetPosition() const
{
	return m_position;
}

//-----------------------------------------------------------------------------------------------
void Camera::SetOrientation( const EulerAngles& orientation )
{
	m_orientation = orientation;
}

//-----------------------------------------------------------------------------------------------
EulerAngles Camera::GetOrientation() const
{
 return m_orientation;
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
void Camera::SetCameraToRenderTransform( const Mat44& m )
{
	m_cameraToRenderTransform = m;
}

//------------------------------------------------------------------------------
Mat44 Camera::GetCameraToRenderTransform() const
{
	return m_cameraToRenderTransform; 
}

//------------------------------------------------------------------------------
Mat44 Camera::GetRenderToClipTransform() const
{
	return GetProjectionMatrix();
}

//------------------------------------------------------------------------------
Vec2 Camera::GetOrthographicBottomLeft() const
{
	return m_orthographicBottomLeft;
}

//------------------------------------------------------------------------------
Vec2 Camera::GetOrthographicTopRight() const
{
	return m_orthographicTopRight;
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
	return GetPerspectiveMatrix();
}

//------------------------------------------------------------------------------
void Camera::SetViewport( AABB2 const& viewport )
{
	m_viewport = viewport;
}

//------------------------------------------------------------------------------
AABB2 Camera::GetViewport() const
{
	return m_viewport;
}
