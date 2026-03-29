#include "Game/TestShapeCylinder.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Math/MathUtils.hpp"

TestShapeCylinder::TestShapeCylinder( const Vec3& start, const Vec3& end, float radius, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/, int numSlices /*= 32 */ )
	: m_radius( radius )
	, m_center( ( start + end ) * 0.5f )
	, m_zRange( start.z - radius, end.z + radius )
{
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
	return false; // #todo Implement this function
}

