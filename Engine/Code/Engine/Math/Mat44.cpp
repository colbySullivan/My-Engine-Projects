#include "Engine/Math/Mat44.hpp"
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
Mat44::Mat44( Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& translation3D )
{
	m_values[Ix] = iBasis3D.x;	m_values[Jx] = jBasis3D.x;	m_values[Kx] = 0.0f;	m_values[Tx] = translation3D.x;
	m_values[Iy] = iBasis3D.y;	m_values[Jy] = jBasis3D.y;	m_values[Ky] = 0.0f;	m_values[Ty] = translation3D.y;
	m_values[Iz] = iBasis3D.z;	m_values[Jz] = jBasis3D.z;	m_values[Kz] = 1.0f;	m_values[Tz] = translation3D.z;
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
	Mat44 result( iBasis3D, jBasis3D, translationXYZ );
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
	Vec3 iBasis3D( 1.f * uniformScaleXYZ, 0.f, 0.f );
	Vec3 jBasis3D( 0.f, 1.f * uniformScaleXYZ, 0.f );
	Mat44 result( iBasis3D, jBasis3D, Vec3( 0.f, 0.f, 0.f ) );
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
	Mat44 result( iBasis3D, jBasis3D, Vec3( 0.f, 0.f, 0.f ) );
	return result;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeZRotationDegrees( float rotationDegreesAboutZ )
{
	Mat44 result;
	return result;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeYRotationDegrees( float rotationDegreesAboutY )
{
	Mat44 result;
	return result;
}

//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeXRotationDegrees( float rotationDegreesAboutX )
{
	Mat44 result;
	return result;
}

//-----------------------------------------------------------------------------------------------
Vec2 const Mat44::TransformVectorQuantity2D( Vec2 const& vectorQuantityXY ) const
{
	return Vec2( 0.f, 0.f );
}

//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::TransformVectorQuantity3D( Vec3 const& vectorQuantityXYZ ) const
{
	return Vec3( 0.f, 0.f, 0.f );
}

//-----------------------------------------------------------------------------------------------
Vec2 const Mat44::TransformPosition2D( Vec2 const& positionXY ) const
{
	return Vec2( 0.f, 0.f );
}

//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::TransformPosition3D( Vec3 const& position3D ) const
{
	return Vec3( 0.f, 0.f, 0.f );
}

//-----------------------------------------------------------------------------------------------
Vec4 const Mat44::TransformHomogeneous3D( Vec4 const& homogeneousPoint3D ) const
{
	return Vec4( 0.f, 0.f, 0.f, 0.f );
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
void Mat44::SetTranslation2D( Vec2 const& translationXY )
{

}

//-----------------------------------------------------------------------------------------------
void Mat44::SetTranslation3D( Vec3 const& translationXYZ )
{

}

//-----------------------------------------------------------------------------------------------
void Mat44::SetIJ2D( Vec2 const& iBasis2D, Vec2 const& jBasis2D )
{

}

//-----------------------------------------------------------------------------------------------
void Mat44::SetIJ12D( Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translationXY )
{

}

//-----------------------------------------------------------------------------------------------
void Mat44::SetIJK3D( Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D )
{

}

//-----------------------------------------------------------------------------------------------
void Mat44::SetIJKT3D( Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translationXYZ )
{

}

//-----------------------------------------------------------------------------------------------
void Mat44::SetIJKT4D( Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D )
{

}

//-----------------------------------------------------------------------------------------------
void Mat44::Append( Mat44 const& appendThis )
{

}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendZRotation( float degreesRotationAboutZ )
{

}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendYRotation( float degreesRotationAboutY )
{

}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendXRotation( float degreesRotationAboutX )
{

}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendTranslation2D( Vec2 const& translationXY )
{

}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendTranslation3D( Vec3 const& translationXYZ )
{

}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendScaleUniform2D( float uniformScaleXY )
{

}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendScaleUniform3D( float uniformScaleXYZ )
{

}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendScaleNonUniform2D( Vec2 const& nonUniformScaleXY )
{

}

//-----------------------------------------------------------------------------------------------
void Mat44::AppendScaleNonUniform3D( Vec3 const& nonUniformScaleXYZ )
{

}
