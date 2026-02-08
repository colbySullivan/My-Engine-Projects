#include "Engine/Math/EulerAngles.hpp"

//------------------------------------------------------------------------------
EulerAngles::EulerAngles(float yawDegrees, float pitchDegrees, float rollDegrees)
	: m_yawDegrees(yawDegrees)
	, m_pitchDegrees(pitchDegrees)
	, m_rollDegrees(rollDegrees)
{

}

//------------------------------------------------------------------------------
Vec3 EulerAngles::GetForwardDir_IFwd_JLeft_KUp() const
{
	float cosY = CosDegrees(m_yawDegrees);
	float sinY = SinDegrees(m_yawDegrees);
	float cosP = CosDegrees(m_pitchDegrees);
	float sinP = SinDegrees(m_pitchDegrees);
	return Vec3(cosY * cosP, sinY * cosP, -sinP);
}

//------------------------------------------------------------------------------
void EulerAngles::GetAsVectors_IFwd_JLeft_KUp(Vec3& out_forwardIBasis, Vec3& out_leftJBasis, Vec3& out_upKBasis) const
{
#if defined(USE_MAT44_FOR_EULER_ANGLES)
	Mat44 eulerMat;
	eulerMat.AppendZRotation(m_yawDegrees);
	eulerMat.AppendYRotation(m_pitchDegrees);
	eulerMat.AppendXRotation(m_rollDegrees);
	out_forwardIBasis = eulerMat.GetIBasis3D();
	out_leftJBasis = eulerMat.GetJBasis3D();
	out_upKBasis = eulerMat.GetKBasis3D();
#else
	float cosY = CosDegrees( m_yawDegrees );
	float sinY = SinDegrees( m_yawDegrees );
	float cosP = CosDegrees( m_pitchDegrees );
	float sinP = SinDegrees( m_pitchDegrees );
	float cosR = CosDegrees( m_rollDegrees );
	float sinR = SinDegrees( m_rollDegrees );

	out_forwardIBasis = Vec3( cosY * cosP, sinY * cosP, -sinP );
	out_leftJBasis = Vec3( ( cosY * sinP * sinR ) - ( sinY * cosR ), ( sinY * sinP * sinR ) + ( cosY * cosR ), cosP * sinR );
	out_upKBasis = Vec3( ( cosY * sinP * cosR ) + ( sinY * sinR ), ( sinY * sinP * cosR ) - ( cosY * sinR ), cosP * cosR );
#endif
}

//------------------------------------------------------------------------------
Mat44 EulerAngles::GetAsMatrix_IFwd_JLeft_KUp() const
{
#if defined(USE_MAT44_FOR_EULER_ANGLES)
	Mat44 eulerMat;
	eulerMat.AppendZRotation(m_yawDegrees);
	eulerMat.AppendYRotation(m_pitchDegrees);
	eulerMat.AppendXRotation(m_rollDegrees);
	return eulerMat;
#else
	float cosY = CosDegrees( m_yawDegrees );
	float sinY = SinDegrees( m_yawDegrees );
	float cosP = CosDegrees( m_pitchDegrees );
	float sinP = SinDegrees( m_pitchDegrees );
	float cosR = CosDegrees( m_rollDegrees );
	float sinR = SinDegrees( m_rollDegrees );
	Mat44 eulerMat;
	eulerMat.SetIJK3D(  Vec3( cosY * cosP, sinY * cosP, -sinP ),  
						Vec3( ( cosY * sinP * sinR ) - ( sinY * cosR ), ( sinY * sinP * sinR ) + ( cosY * cosR ), cosP * sinR ), 
						Vec3( ( cosY * sinP * cosR ) + ( sinY * sinR ), ( sinY * sinP * cosR ) - ( cosY * sinR ), cosP * cosR ) );
	return eulerMat;
#endif
}

//------------------------------------------------------------------------------
void EulerAngles::operator+=(EulerAngles const& anglesToAdd)
{
	m_yawDegrees += anglesToAdd.m_yawDegrees;
	m_pitchDegrees += anglesToAdd.m_pitchDegrees;
	m_rollDegrees += anglesToAdd.m_rollDegrees;
}

//------------------------------------------------------------------------------
EulerAngles const Interpolate( EulerAngles const& from, EulerAngles const& to, float lerpFraction )
{
	EulerAngles result;
	result.m_yawDegrees = InterpolateShortestDis(from.m_yawDegrees, to.m_yawDegrees, lerpFraction);
	result.m_pitchDegrees = InterpolateShortestDis(from.m_pitchDegrees, to.m_pitchDegrees, lerpFraction);
	result.m_rollDegrees = InterpolateShortestDis(from.m_rollDegrees, to.m_rollDegrees, lerpFraction);
	return result;
}
