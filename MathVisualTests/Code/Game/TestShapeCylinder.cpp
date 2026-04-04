#include "Game/TestShapeCylinder.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Math/MathUtils.hpp"

TestShapeCylinder::TestShapeCylinder( const Vec3& start, const Vec3& end, float radius, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/, int numSlices /*= 32 */ )
	: m_radius( radius )
	, m_center( ( start + end ) * 0.5f )
{
	m_zRange.m_min = fminf( start.z, end.z );
	m_zRange.m_max = fmaxf( start.z, end.z );
	AddVertsForCylinder3D( m_cylinderVerts, start, end, radius, color, UVs, numSlices );
}

TestShapeCylinder::~TestShapeCylinder()
{

}

void TestShapeCylinder::Render() const
{
	IsOverlapped();
	g_engine->m_render->m_desiredBlendMode = BlendMode::OPAQUE;
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	g_engine->m_render->BindTexture( nullptr );
	g_engine->m_render->DrawVertexArray( ( int )m_cylinderVerts.size(), m_cylinderVerts.data() );
}

void TestShapeCylinder::RenderWithTexture( Texture* texture ) const
{
	IsOverlapped();
	g_engine->m_render->m_desiredBlendMode = BlendMode::OPAQUE;
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	g_engine->m_render->BindTexture( texture );
	g_engine->m_render->DrawVertexArray( ( int )m_cylinderVerts.size(), m_cylinderVerts.data() );
}

//------------------------------------------------------------------------------
RaycastResult3D TestShapeCylinder::RaycastTestShape( Vec3 startPos, Vec3 forwardNormal, float maxDistance ) const
{
	RaycastResult3D result = RaycastVsCylinder( startPos, forwardNormal, maxDistance, Vec3( m_center.x, m_center.y, m_zRange.m_min ), Vec3( m_center.x, m_center.y, m_zRange.m_max ), m_radius );
	return result;
}

//------------------------------------------------------------------------------
bool TestShapeCylinder::DoesSphereOverlap( Vec3 sphereCenter, float radius ) const
{
	return DoSphereCylinderOverlap( sphereCenter, radius, Vec3( m_center.x, m_center.y, m_zRange.m_min ), Vec3( m_center.x, m_center.y, m_zRange.m_max ), m_radius );
}

//------------------------------------------------------------------------------
bool TestShapeCylinder::DoesCylinderOverlap( Vec2 cylinderCenter, float cylinderRadius, FloatRange cylinderZRange ) const
{
	return DoCylindersOverlap( Vec3( cylinderCenter.x, cylinderCenter.y, cylinderZRange.m_min ), Vec3( cylinderCenter.x, cylinderCenter.y, cylinderZRange.m_max ), cylinderRadius, Vec3( m_center.x, m_center.y, m_zRange.m_min ), Vec3( m_center.x, m_center.y, m_zRange.m_max ), m_radius );
}

//------------------------------------------------------------------------------
bool TestShapeCylinder::DoesAABBOverlap( Vec3 aabbMins, Vec3 aabbMaxs ) const
{
	Vec2 cylinderCenter = Vec2( m_center.x, m_center.y );
	return DoAABB3sCylinderOverlap( aabbMins, aabbMaxs, cylinderCenter, m_radius, m_zRange );
}

Vec3 TestShapeCylinder::GetClosestPoint( Vec3 referencePos ) const
{
	return GetNearestPointOnCylinder( referencePos, Vec3( m_center.x, m_center.y, m_zRange.m_min ), Vec3( m_center.x, m_center.y, m_zRange.m_max ), m_radius );
}

