#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"
//m_values[Ix]

// This is for append
//Mat44 copyOfThis = *this;
//left and right = &copyOfThis.m_values[0];

//-----------------------------------------------------------------------------------------------
Mat44::Mat44()
{
	m_values[Ix] = 1.0f;	m_values[Jx] = 0.0f;	m_values[Kx] = 0.0f;	m_values[Tx] = 0.0f;
	m_values[Iy] = 0.0f;	m_values[Jy] = 1.0f;	m_values[Ky] = 0.0f;	m_values[Ty] = 0.0f;
	m_values[Iz] = 0.0f;	m_values[Jz] = 0.0f;	m_values[Kz] = 1.0f;	m_values[Tz] = 0.0f;
	m_values[Iw] = 0.0f;	m_values[Jw] = 0.0f;	m_values[Kw] = 0.0f;	m_values[Tw] = 1.0f;
}

//-----------------------------------------------------------------------------------------------
Mat44::Mat44( Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translation2D )
{
	m_values[Ix] = iBasis2D.x;	m_values[Jx] = jBasis2D.x;	m_values[Kx] = 0.0f;	m_values[Tx] = translation2D.x;
	m_values[Iy] = iBasis2D.y;	m_values[Jy] = jBasis2D.y;	m_values[Ky] = 0.0f;	m_values[Ty] = translation2D.y;
	m_values[Iz] = 0.0f;		m_values[Jz] = 0.0f;		m_values[Kz] = 1.0f;	m_values[Tz] = 0.0f;
	m_values[Iw] = 0.0f;		m_values[Jw] = 0.0f;		m_values[Kw] = 0.0f;	m_values[Tw] = 1.0f;
}

//-----------------------------------------------------------------------------------------------
Mat44::Mat44( Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translation3D )
{
	m_values[Ix] = iBasis3D.x;	m_values[Jx] = jBasis3D.x;	m_values[Kx] = kBasis3D.x;	m_values[Tx] = translation3D.x;
	m_values[Iy] = iBasis3D.y;	m_values[Jy] = jBasis3D.y;	m_values[Ky] = kBasis3D.y;	m_values[Ty] = translation3D.y;
	m_values[Iz] = iBasis3D.z;	m_values[Jz] = jBasis3D.z;	m_values[Kz] = kBasis3D.z;	m_values[Tz] = translation3D.z;
	m_values[Iw] = 0.0f;		m_values[Jw] = 0.0f;		m_values[Kw] = 0.0f;		m_values[Tw] = 1.0f;
}

//-----------------------------------------------------------------------------------------------
Mat44::Mat44( Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D )
{
	m_values[Ix] = iBasis4D.x;	m_values[Jx] = jBasis4D.x;	m_values[Kx] = kBasis4D.x;	m_values[Tx] = translation4D.x;
	m_values[Iy] = iBasis4D.y;	m_values[Jy] = jBasis4D.y;	m_values[Ky] = kBasis4D.y;	m_values[Ty] = translation4D.y;
	m_values[Iz] = iBasis4D.z;	m_values[Jz] = jBasis4D.z;	m_values[Kz] = kBasis4D.z;	m_values[Tz] = translation4D.z;
	m_values[Iw] = iBasis4D.w;	m_values[Jw] = jBasis4D.w;	m_values[Kw] = kBasis4D.w;	m_values[Tw] = translation4D.w;
}

//-----------------------------------------------------------------------------------------------
Mat44::Mat44( float const* sixteenValuesBasisMajor )
{
	for (int i = 0; i < 16 ; ++i)
	{
		m_values[i] = sixteenValuesBasisMajor[i];
	}
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeTranslation2D( Vec2 const& translationXY )
{
	//result.m_values[Ix] = 1.0f;  result.m_values[Jx] = 0.0f;  result.m_values[Kx] = 0.0f;  result.m_values[Tx] = translationXY.x;
	//result.m_values[Iy] = 0.0f;  result.m_values[Jy] = 1.0f;  result.m_values[Ky] = 0.0f;  result.m_values[Ty] = translationXY.y;
	//result.m_values[Iz] = 0.0f;  result.m_values[Jz] = 0.0f;  result.m_values[Kz] = 1.0f;  result.m_values[Tz] = 0.0f;
	//result.m_values[Iw] = 0.0f;  result.m_values[Jw] = 0.0f;  result.m_values[Kw] = 0.0f;  result.m_values[Tw] = 1.0f;
	Vec2 iBasis2D( 1.f, 0.f );
	Vec2 jBasis2D( 0.f, 1.f );
	Mat44 result( iBasis2D, jBasis2D, translationXY );
	return result;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeTranslation3D( Vec3 const& translationXYZ )
{
	Vec3 iBasis3D( 1.f, 0.f, 0.f );
	Vec3 jBasis3D( 0.f, 1.f, 0.f );
	Vec3 kBasis3D( 0.f, 0.f, 1.f );
	Mat44 result( iBasis3D, jBasis3D, kBasis3D, translationXYZ );
	return result;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeUniformScale2D( float uniformScaleXY )
{
	Vec2 iBasis2D( 1.f * uniformScaleXY, 0.f );
	Vec2 jBasis2D( 0.f, 1.f * uniformScaleXY );
	Mat44 result( iBasis2D, jBasis2D, Vec2( 0.f, 0.f ) );
	return result;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeUniformScale3D( float uniformScaleXYZ )
{
	Vec3 iBasis3D( uniformScaleXYZ, 0.f, 0.f );
	Vec3 jBasis3D( 0.f, uniformScaleXYZ, 0.f );
	Vec3 kBasis3D( 0.f, 0.f, uniformScaleXYZ );
	Vec3 tBasis3D( 0.f, 0.f, 0.f );
	Mat44 result( iBasis3D, jBasis3D, kBasis3D, tBasis3D );
	return result;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeNonUniformScale2D( Vec2 const& nonUniformScaleXY )
{
	Vec2 iBasis2D( 1.f * nonUniformScaleXY.x, 0.f );
	Vec2 jBasis2D( 0.f, 1.f * nonUniformScaleXY.y );
	Mat44 result( iBasis2D, jBasis2D, Vec2( 0.f, 0.f ) );
	return result;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeNonUniformScale3D( Vec3 const& nonUniformScaleXYZ )
{
	Vec3 iBasis3D( 1.f * nonUniformScaleXYZ.x, 0.f, 0.f );
	Vec3 jBasis3D( 0.f, 1.f * nonUniformScaleXYZ.y, 0.f );
	Vec3 kBasis3D( 0.f, 0.f, 1.f * nonUniformScaleXYZ.z );
	Vec3 tBasis3D( 0.f, 0.f, 0.f );
	Mat44 result( iBasis3D, jBasis3D, kBasis3D, tBasis3D );
	return result;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeZRotationDegrees( float rotationDegreesAboutZ )
{
	float cosDeg = CosDegrees( rotationDegreesAboutZ );
	float sinDeg = SinDegrees( rotationDegreesAboutZ );
	Vec3 iBasis3D( cosDeg, sinDeg, 0.f );
	Vec3 jBasis3D( -sinDeg, cosDeg, 0.f );
	Vec3 kBasis3D( 0.f, 0.f, 1.f );
	Vec3 tBasis3D( 0.f, 0.f, 0.f );
	Mat44 result( iBasis3D, jBasis3D, kBasis3D, tBasis3D );
	return result;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeYRotationDegrees( float rotationDegreesAboutY )
{
	float cosDeg = CosDegrees( rotationDegreesAboutY );
	float sinDeg = SinDegrees( rotationDegreesAboutY );
	Vec3 iBasis3D( cosDeg, 0.f, -sinDeg );
	Vec3 jBasis3D( 0.f, 1.f, 0.f );
	Vec3 kBasis3D( sinDeg, 0.f, cosDeg );
	Vec3 tBasis3D( 0.f, 0.f, 0.f );
	Mat44 result( iBasis3D, jBasis3D, kBasis3D, tBasis3D );
	return result;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeXRotationDegrees( float rotationDegreesAboutX )
{
	float cosDeg = CosDegrees( rotationDegreesAboutX );
	float sinDeg = SinDegrees( rotationDegreesAboutX );
	Vec3 iBasis3D( 1.f, 0.f, 0.f );
	Vec3 jBasis3D( 0.f, cosDeg, sinDeg );
	Vec3 kBasis3D( 0.f, -sinDeg, cosDeg );
	Vec3 tBasis3D( 0.f, 0.f, 0.f );
	Mat44 result( iBasis3D, jBasis3D, kBasis3D, tBasis3D );
	return result;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeOrthoProjection( float left, float right, float bottom, float top, float zNear, float zFar )
{
	float sOfX = (2 / ( right - left ));
	float tOfX = (left + right) / (left - right);
	float sOfY = (2 / ( top - bottom ));
	float tOfY = ( bottom + top ) / ( bottom - top );
	float sOfZ = ( 1 / ( zFar - zNear ) );
	float tOfZ = zNear / ( zNear - zFar );

	Vec3 iBasis3D( sOfX, 0.f, 0.f );
	Vec3 jBasis3D( 0.f, sOfY, 0.f );
	Vec3 kBasis3D( 0.f, 0.f, sOfZ );
	Vec3 translationXYZ( tOfX, tOfY, tOfZ );

	Mat44 result( iBasis3D, jBasis3D, kBasis3D, translationXYZ );
	return result;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakePerspectiveProjection( float fovYDegrees, float aspect, float zNear, float zFar )
{
	Mat44 perspective;

	float c = CosDegrees( fovYDegrees * 0.5f );
	float s = SinDegrees( fovYDegrees * 0.5f );
	float scaleY = c / s;
	float scaleX = scaleY / aspect;

	float scaleZ = zFar / ( zFar - zNear );
	float translateZ = ( zNear * zFar ) / ( zNear - zFar );

	perspective.m_values[Ix] = scaleX;
	perspective.m_values[Jy] = scaleY;
	perspective.m_values[Kz] = scaleZ;
	perspective.m_values[Kw] = 1.f;
	perspective.m_values[Tz] = translateZ;
	perspective.m_values[Tw] = 0.f;

	return perspective;
}

//-----------------------------------------------------------------------------------------------
Vec2 const Mat44::TransformVectorQuantity2D( Vec2 const& vectorQuantityXY ) const
{
	float x = ( m_values[Ix] * vectorQuantityXY.x ) + ( m_values[Jx] * vectorQuantityXY.y );
	float y = ( m_values[Iy] * vectorQuantityXY.x ) + ( m_values[Jy] * vectorQuantityXY.y );
	return Vec2( x, y );
}

//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::TransformVectorQuantity3D( Vec3 const& vectorQuantityXYZ ) const
{
	float x = ( m_values[Ix] * vectorQuantityXYZ.x ) + ( m_values[Jx] * vectorQuantityXYZ.y ) + ( m_values[Kx] * vectorQuantityXYZ.z );
	float y = ( m_values[Iy] * vectorQuantityXYZ.x ) + ( m_values[Jy] * vectorQuantityXYZ.y ) + ( m_values[Ky] * vectorQuantityXYZ.z );
	float z = ( m_values[Iz] * vectorQuantityXYZ.x ) + ( m_values[Jz] * vectorQuantityXYZ.y ) + ( m_values[Kz] * vectorQuantityXYZ.z );
	return Vec3( x, y, z );
}

//-----------------------------------------------------------------------------------------------
Vec2 const Mat44::TransformPosition2D( Vec2 const& positionXY ) const
{
	float x = ( m_values[Ix] * positionXY.x ) + ( m_values[Jx] * positionXY.y ) + m_values[Tx];
	float y = ( m_values[Iy] * positionXY.x ) + ( m_values[Jy] * positionXY.y ) + m_values[Ty];
	return Vec2( x, y );
}

//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::TransformPosition3D( Vec3 const& position3D ) const
{
	float x = ( m_values[Ix] * position3D.x ) + ( m_values[Jx] * position3D.y ) + ( m_values[Kx] * position3D.z ) + m_values[Tx];
	float y = ( m_values[Iy] * position3D.x ) + ( m_values[Jy] * position3D.y ) + ( m_values[Ky] * position3D.z ) + m_values[Ty];
	float z = ( m_values[Iz] * position3D.x ) + ( m_values[Jz] * position3D.y ) + ( m_values[Kz] * position3D.z ) + m_values[Tz];
	return Vec3( x, y, z );
}

//-----------------------------------------------------------------------------------------------
Vec4 const Mat44::TransformHomogeneous3D( Vec4 const& homogeneousPoint3D ) const
{
	float x = ( m_values[Ix] * homogeneousPoint3D.x ) + ( m_values[Jx] * homogeneousPoint3D.y ) + ( m_values[Kx] * homogeneousPoint3D.z ) + ( m_values[Tx] * homogeneousPoint3D.w );
	float y = ( m_values[Iy] * homogeneousPoint3D.x ) + ( m_values[Jy] * homogeneousPoint3D.y ) + ( m_values[Ky] * homogeneousPoint3D.z ) + ( m_values[Ty] * homogeneousPoint3D.w );
	float z = ( m_values[Iz] * homogeneousPoint3D.x ) + ( m_values[Jz] * homogeneousPoint3D.y ) + ( m_values[Kz] * homogeneousPoint3D.z ) + ( m_values[Tz] * homogeneousPoint3D.w );
	float w = ( m_values[Iw] * homogeneousPoint3D.x ) + ( m_values[Jw] * homogeneousPoint3D.y ) + ( m_values[Kw] * homogeneousPoint3D.z ) + ( m_values[Tw] * homogeneousPoint3D.w );
	return Vec4( x, y, z, w );
}

//-----------------------------------------------------------------------------------------------
float* Mat44::GetAsFloatArray()
{
	return m_values;
}

//-----------------------------------------------------------------------------------------------
float const* Mat44::GetAsFloatArray() const
{
	return m_values;
}

//-----------------------------------------------------------------------------------------------
Vec2 const Mat44::GetIBasis2D() const
{
	return Vec2( m_values[Ix], m_values[Iy] );
}

//-----------------------------------------------------------------------------------------------
Vec2 const Mat44::GetJBasis2D() const
{
	return Vec2( m_values[Jx], m_values[Jy] );
}

//-----------------------------------------------------------------------------------------------
Vec2 const Mat44::GetTranslation2D() const
{
	return Vec2( m_values[Tx], m_values[Ty] );
}

//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::GetIBasis3D() const
{
	return Vec3( m_values[Ix], m_values[Iy], m_values[Iz] );
}

//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::GetJBasis3D() const
{
	return Vec3( m_values[Jx], m_values[Jy], m_values[Jz] );
}

//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::GetKBasis3D() const
{
	return Vec3( m_values[Kx], m_values[Ky], m_values[Kz] );
}

//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::GetTranslation3D() const
{
	return Vec3( m_values[Tx], m_values[Ty], m_values[Tz] );

}

//-----------------------------------------------------------------------------------------------
Vec4 const Mat44::GetIBasis4D() const
{
	return Vec4( m_values[Ix], m_values[Iy], m_values[Iz], m_values[Iw]);
}

//-----------------------------------------------------------------------------------------------
Vec4 const Mat44::GetJBasis4D() const
{
	return Vec4( m_values[Jx], m_values[Jy], m_values[Jz], m_values[Jw] );
}

//-----------------------------------------------------------------------------------------------
Vec4 const Mat44::GetKBasis4D() const
{
	return Vec4( m_values[Kx], m_values[Ky], m_values[Kz], m_values[Kw] );
}

//-----------------------------------------------------------------------------------------------
Vec4 const Mat44::GetTranslation4D() const
{
	return Vec4( m_values[Tx], m_values[Ty], m_values[Tz], m_values[Tw] );
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::GetOrthonormalInverse() const // #TODO GetOrthonormalInverse
{
	Mat44 copy = *this;
	return copy;
}

//-----------------------------------------------------------------------------------------------
void Mat44::SetTranslation2D( Vec2 const& translationXY )
{
	m_values[Tx] = translationXY.x;
	m_values[Ty] = translationXY.y;
	m_values[Tz] = 0.f;
	m_values[Tw] = 1.f;
}

//-----------------------------------------------------------------------------------------------
void Mat44::SetTranslation3D( Vec3 const& translationXYZ )
{
	m_values[Tx] = translationXYZ.x;
	m_values[Ty] = translationXYZ.y;
	m_values[Tz] = translationXYZ.z;
	m_values[Tw] = 1.f;
}

//-----------------------------------------------------------------------------------------------
void Mat44::SetIJ2D( Vec2 const& iBasis2D, Vec2 const& jBasis2D )
{
	m_values[Ix] = iBasis2D.x;
	m_values[Iy] = iBasis2D.y;
	m_values[Iw] = 0.f;
	m_values[Iz] = 0.f;

	m_values[Jx] = jBasis2D.x;
	m_values[Jy] = jBasis2D.y;
	m_values[Jw] = 0.f;
	m_values[Jz] = 0.f;
}

//-----------------------------------------------------------------------------------------------
void Mat44::SetIJT2D( Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translationXY )
{
	m_values[Ix] = iBasis2D.x;	m_values[Jx] = jBasis2D.x;	m_values[Tx] = translationXY.x;
	m_values[Iy] = iBasis2D.y;	m_values[Jy] = jBasis2D.y;	m_values[Ty] = translationXY.y;
	m_values[Iw] = 0.f;			m_values[Jw] = 0.f;			m_values[Tw] = 1.f;
	m_values[Iz] = 0.f;			m_values[Jz] = 0.f;			m_values[Tz] = 0.f;
}

//-----------------------------------------------------------------------------------------------
void Mat44::SetIJK3D( Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D )
{
	m_values[Ix] = iBasis3D.x;	m_values[Jx] = jBasis3D.x;	m_values[Kx] = kBasis3D.x;
	m_values[Iy] = iBasis3D.y;	m_values[Jy] = jBasis3D.y;	m_values[Ky] = kBasis3D.y;
	m_values[Iz] = iBasis3D.z;	m_values[Jz] = jBasis3D.z;	m_values[Kz] = kBasis3D.z;
	m_values[Iw] = 0.f;			m_values[Jw] = 0.f;			m_values[Kw] = 0.f;
}

//-----------------------------------------------------------------------------------------------
void Mat44::SetIJKT3D( Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translationXYZ )
{
	m_values[Ix] = iBasis3D.x;	m_values[Jx] = jBasis3D.x;	m_values[Kx] = kBasis3D.x;	m_values[Tx] = translationXYZ.x;
	m_values[Iy] = iBasis3D.y;	m_values[Jy] = jBasis3D.y;	m_values[Ky] = kBasis3D.y;	m_values[Ty] = translationXYZ.y;
	m_values[Iz] = iBasis3D.z;	m_values[Jz] = jBasis3D.z;	m_values[Kz] = kBasis3D.z;	m_values[Tz] = translationXYZ.z;
	m_values[Iw] = 0.f;			m_values[Jw] = 0.f;			m_values[Kw] = 0.f;			m_values[Tw] = 1.f;
}

//-----------------------------------------------------------------------------------------------
void Mat44::SetIJKT4D( Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D )
{
	m_values[Ix] = iBasis4D.x;	m_values[Jx] = jBasis4D.x;	m_values[Kx] = kBasis4D.x;	m_values[Tx] = translation4D.x;
	m_values[Iy] = iBasis4D.y;	m_values[Jy] = jBasis4D.y;	m_values[Ky] = kBasis4D.y;	m_values[Ty] = translation4D.y;
	m_values[Iz] = iBasis4D.z;	m_values[Jz] = jBasis4D.z;	m_values[Kz] = kBasis4D.z;	m_values[Tz] = translation4D.z;
	m_values[Iw] = iBasis4D.w;	m_values[Jw] = jBasis4D.w;	m_values[Kw] = kBasis4D.w;	m_values[Tw] = translation4D.w;
}

//-----------------------------------------------------------------------------------------------
void Mat44::Transpose() // #TODO Transpose
{

}

//-----------------------------------------------------------------------------------------------
void Mat44::Orthonormalize_XFwd_YLeft_ZUp() // #TODO Orthonormalize_XFwd_YLeft_ZUp
{
	Vec3 originalIBasis = GetIBasis3D();
	Vec3 normalizedI = originalIBasis.GetNormalized();

	Vec3 originalKBasis = GetKBasis3D();
	Vec3 kOfI = normalizedI * ( DotProduct3D(originalKBasis, normalizedI) );
	Vec3 newNormalizedK = originalKBasis - kOfI;
	newNormalizedK = newNormalizedK.GetNormalized();

	Vec3 originalJBasis = GetJBasis3D();
	Vec3 jOfI = normalizedI * ( DotProduct3D(originalJBasis, normalizedI) );
	Vec3 newJ = originalJBasis - jOfI;

	Vec3 jOfK = newNormalizedK * ( DotProduct3D( originalJBasis, newNormalizedK ) );
	newJ = newJ - jOfK;
	Vec3 newNormalizedJ = newJ.GetNormalized();

	SetIJK3D(normalizedI, newNormalizedJ, newNormalizedK); 
}

//-----------------------------------------------------------------------------------------------
void Mat44::Append( Mat44 const& appendThis )
{
	Mat44 copy = *this;

	m_values[Ix] = ( copy.m_values[Ix] * appendThis.m_values[Ix] ) + ( copy.m_values[Jx] * appendThis.m_values[Iy] ) + ( copy.m_values[Kx] * appendThis.m_values[Iz] ) + ( copy.m_values[Tx] * appendThis.m_values[Iw] );
	m_values[Jx] = ( copy.m_values[Ix] * appendThis.m_values[Jx] ) + ( copy.m_values[Jx] * appendThis.m_values[Jy] ) + ( copy.m_values[Kx] * appendThis.m_values[Jz] ) + ( copy.m_values[Tx] * appendThis.m_values[Jw] );
	m_values[Kx] = ( copy.m_values[Ix] * appendThis.m_values[Kx] ) + ( copy.m_values[Jx] * appendThis.m_values[Ky] ) + ( copy.m_values[Kx] * appendThis.m_values[Kz] ) + ( copy.m_values[Tx] * appendThis.m_values[Kw] );
	m_values[Tx] = ( copy.m_values[Ix] * appendThis.m_values[Tx] ) + ( copy.m_values[Jx] * appendThis.m_values[Ty] ) + ( copy.m_values[Kx] * appendThis.m_values[Tz] ) + ( copy.m_values[Tx] * appendThis.m_values[Tw] );

	m_values[Iy] = ( copy.m_values[Iy] * appendThis.m_values[Ix] ) + ( copy.m_values[Jy] * appendThis.m_values[Iy] ) + ( copy.m_values[Ky] * appendThis.m_values[Iz] ) + ( copy.m_values[Ty] * appendThis.m_values[Iw] );
	m_values[Jy] = ( copy.m_values[Iy] * appendThis.m_values[Jx] ) + ( copy.m_values[Jy] * appendThis.m_values[Jy] ) + ( copy.m_values[Ky] * appendThis.m_values[Jz] ) + ( copy.m_values[Ty] * appendThis.m_values[Jw] );
	m_values[Ky] = ( copy.m_values[Iy] * appendThis.m_values[Kx] ) + ( copy.m_values[Jy] * appendThis.m_values[Ky] ) + ( copy.m_values[Ky] * appendThis.m_values[Kz] ) + ( copy.m_values[Ty] * appendThis.m_values[Kw] );
	m_values[Ty] = ( copy.m_values[Iy] * appendThis.m_values[Tx] ) + ( copy.m_values[Jy] * appendThis.m_values[Ty] ) + ( copy.m_values[Ky] * appendThis.m_values[Tz] ) + ( copy.m_values[Ty] * appendThis.m_values[Tw] );
	
	m_values[Iz] = ( copy.m_values[Iz] * appendThis.m_values[Ix] ) + ( copy.m_values[Jz] * appendThis.m_values[Iy] ) + ( copy.m_values[Kz] * appendThis.m_values[Iz] ) + ( copy.m_values[Tz] * appendThis.m_values[Iw] );
	m_values[Jz] = ( copy.m_values[Iz] * appendThis.m_values[Jx] ) + ( copy.m_values[Jz] * appendThis.m_values[Jy] ) + ( copy.m_values[Kz] * appendThis.m_values[Jz] ) + ( copy.m_values[Tz] * appendThis.m_values[Jw] );
	m_values[Kz] = ( copy.m_values[Iz] * appendThis.m_values[Kx] ) + ( copy.m_values[Jz] * appendThis.m_values[Ky] ) + ( copy.m_values[Kz] * appendThis.m_values[Kz] ) + ( copy.m_values[Tz] * appendThis.m_values[Kw] );
	m_values[Tz] = ( copy.m_values[Iz] * appendThis.m_values[Tx] ) + ( copy.m_values[Jz] * appendThis.m_values[Ty] ) + ( copy.m_values[Kz] * appendThis.m_values[Tz] ) + ( copy.m_values[Tz] * appendThis.m_values[Tw] );
	
	m_values[Iw] = ( copy.m_values[Iw] * appendThis.m_values[Ix] ) + ( copy.m_values[Jw] * appendThis.m_values[Iy] ) + ( copy.m_values[Kw] * appendThis.m_values[Iz] ) + ( copy.m_values[Tw] * appendThis.m_values[Iw] );
	m_values[Jw] = ( copy.m_values[Iw] * appendThis.m_values[Jx] ) + ( copy.m_values[Jw] * appendThis.m_values[Jy] ) + ( copy.m_values[Kw] * appendThis.m_values[Jz] ) + ( copy.m_values[Tw] * appendThis.m_values[Jw] );
	m_values[Kw] = ( copy.m_values[Iw] * appendThis.m_values[Kx] ) + ( copy.m_values[Jw] * appendThis.m_values[Ky] ) + ( copy.m_values[Kw] * appendThis.m_values[Kz] ) + ( copy.m_values[Tw] * appendThis.m_values[Kw] );
	m_values[Tw] = ( copy.m_values[Iw] * appendThis.m_values[Tx] ) + ( copy.m_values[Jw] * appendThis.m_values[Ty] ) + ( copy.m_values[Kw] * appendThis.m_values[Tz] ) + ( copy.m_values[Tw] * appendThis.m_values[Tw] );
}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendZRotation(float degreesRotationAboutZ)
{
	float cosDeg = CosDegrees(degreesRotationAboutZ);
	float sinDeg = SinDegrees(degreesRotationAboutZ);

	float oldIx = m_values[Ix];
	float oldIy = m_values[Iy];
	float oldIz = m_values[Iz];
	float oldIw = m_values[Iw];

	float oldJx = m_values[Jx];
	float oldJy = m_values[Jy];
	float oldJz = m_values[Jz];
	float oldJw = m_values[Jw];

	m_values[Ix] = (oldIx * cosDeg) + (oldJx * sinDeg);
	m_values[Iy] = (oldIy * cosDeg) + (oldJy * sinDeg);
	m_values[Iz] = (oldIz * cosDeg) + (oldJz * sinDeg);
	m_values[Iw] = (oldIw * cosDeg) + (oldJw * sinDeg);

	m_values[Jx] = (oldIx * -sinDeg) + (oldJx * cosDeg);
	m_values[Jy] = (oldIy * -sinDeg) + (oldJy * cosDeg);
	m_values[Jz] = (oldIz * -sinDeg) + (oldJz * cosDeg);
	m_values[Jw] = (oldIw * -sinDeg) + (oldJw * cosDeg);
}


//-----------------------------------------------------------------------------------------------
void Mat44::AppendYRotation( float degreesRotationAboutY )
{
	float cosDeg = CosDegrees( degreesRotationAboutY );
	float sinDeg = SinDegrees( degreesRotationAboutY );

	float oldIx = m_values[Ix];
	float oldIy = m_values[Iy];
	float oldIz = m_values[Iz];
	float oldIw = m_values[Iw];

	float oldKx = m_values[Kx];
	float oldKy = m_values[Ky];
	float oldKz = m_values[Kz];
	float oldKw = m_values[Kw];

	m_values[Ix] = ( oldIx * cosDeg ) - ( oldKx * sinDeg );
	m_values[Iy] = ( oldIy * cosDeg ) - ( oldKy * sinDeg );
	m_values[Iz] = ( oldIz * cosDeg ) - ( oldKz * sinDeg );
	m_values[Iw] = ( oldIw * cosDeg ) - ( oldKw * sinDeg );

	m_values[Kx] = ( oldIx * sinDeg ) + ( oldKx * cosDeg );
	m_values[Ky] = ( oldIy * sinDeg ) + ( oldKy * cosDeg );
	m_values[Kz] = ( oldIz * sinDeg ) + ( oldKz * cosDeg );
	m_values[Kw] = ( oldIw * sinDeg ) + ( oldKw * cosDeg );
}


//-----------------------------------------------------------------------------------------------
void Mat44::AppendXRotation( float degreesRotationAboutX )
{
	Mat44 rotationMatrix = MakeXRotationDegrees( degreesRotationAboutX );
	Append( rotationMatrix );
}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendTranslation2D( Vec2 const& translationXY )
{
	Mat44 translationMatrix = MakeTranslation2D( translationXY );
	Append( translationMatrix );
}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendTranslation3D( Vec3 const& translationXYZ )
{
	Mat44 translationMatrix = MakeTranslation3D( translationXYZ );
	Append( translationMatrix );
}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendScaleUniform2D( float uniformScaleXY )
{
	Mat44 scaleMatrix = MakeUniformScale2D( uniformScaleXY );
	Append( scaleMatrix );
}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendScaleUniform3D( float uniformScaleXYZ )
{
	Mat44 scaleMatrix = MakeUniformScale3D( uniformScaleXYZ );
	Append( scaleMatrix );
}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendScaleNonUniform2D( Vec2 const& nonUniformScaleXY )
{
	Mat44 scaleMatrix = MakeNonUniformScale2D( nonUniformScaleXY );
	Append( scaleMatrix );
}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendScaleNonUniform3D( Vec3 const& nonUniformScaleXYZ )
{
	Mat44 scaleMatrix = MakeNonUniformScale3D( nonUniformScaleXYZ );
	Append( scaleMatrix );
}
