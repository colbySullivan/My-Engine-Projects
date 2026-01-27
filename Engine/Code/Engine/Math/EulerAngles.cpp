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
	Mat44 eulerMat;
	eulerMat.AppendZRotation(m_yawDegrees);
	eulerMat.AppendYRotation(m_pitchDegrees);
	eulerMat.AppendXRotation(m_rollDegrees);
	out_forwardIBasis = eulerMat.GetIBasis3D();
	out_leftJBasis = eulerMat.GetJBasis3D();
	out_upKBasis = eulerMat.GetKBasis3D();
}

//------------------------------------------------------------------------------
Mat44 EulerAngles::GetAsMatrix_IFwd_JLeft_KUp() const
{
	Mat44 eulerMat;
	eulerMat.AppendZRotation(m_yawDegrees);
	eulerMat.AppendYRotation(m_pitchDegrees);
	eulerMat.AppendXRotation(m_rollDegrees);
	return eulerMat;
}

//------------------------------------------------------------------------------
void EulerAngles::operator+=(EulerAngles const& anglesToAdd)
{

}

//------------------------------------------------------------------------------
EulerAngles const Interpolate( EulerAngles const& from, EulerAngles const& to, float lerpFraction )
{
	EulerAngles result;
	result.m_yawDegrees = Interpolate(from.m_yawDegrees, to.m_yawDegrees, lerpFraction);
	result.m_pitchDegrees = Interpolate(from.m_pitchDegrees, to.m_pitchDegrees, lerpFraction);
	result.m_pitchDegrees = Interpolate(from.m_rollDegrees, to.m_rollDegrees, lerpFraction);
	return result;
}
