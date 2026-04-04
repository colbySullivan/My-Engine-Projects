#include "Game/TestShapeAABB3.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Math/MathUtils.hpp"

//------------------------------------------------------------------------------
TestShapeAABB3::TestShapeAABB3( const AABB3& bounds, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE */ )
	: m_bounds( bounds )
{
	AddVertsForAABB3D( m_aabb3Verts, m_bounds, color, UVs );
}

//------------------------------------------------------------------------------
TestShapeAABB3::~TestShapeAABB3()
{
	m_aabb3Verts.clear();
}

//------------------------------------------------------------------------------
void TestShapeAABB3::Render() const
{
	IsOverlapped();
	g_engine->m_render->m_desiredBlendMode = BlendMode::OPAQUE;
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	g_engine->m_render->BindTexture( nullptr );
	g_engine->m_render->DrawVertexArray( ( int )m_aabb3Verts.size(), m_aabb3Verts.data() );
}

void TestShapeAABB3::RenderWithTexture( Texture* texture ) const
{
	IsOverlapped();
	g_engine->m_render->m_desiredBlendMode = BlendMode::OPAQUE;
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	g_engine->m_render->BindTexture( texture );
	g_engine->m_render->DrawVertexArray( ( int )m_aabb3Verts.size(), m_aabb3Verts.data() );
}

//------------------------------------------------------------------------------
RaycastResult3D TestShapeAABB3::RaycastTestShape( Vec3 startPos, Vec3 forwardNormal, float maxDistance ) const
{
	RaycastResult3D result;
	return result;
}

//------------------------------------------------------------------------------
bool TestShapeAABB3::DoesSphereOverlap( Vec3 sphereCenter, float radius ) const
{
	return DoSphereABB3Overlap( sphereCenter, radius, m_bounds.m_mins, m_bounds.m_maxs );
}

//------------------------------------------------------------------------------
bool TestShapeAABB3::DoesCylinderOverlap( Vec2 cylinderCenter, float cylinderRadius, FloatRange cylinderZRange ) const
{
	return DoAABB3sCylinderOverlap( m_bounds.m_mins, m_bounds.m_maxs, cylinderCenter, cylinderRadius, cylinderZRange );
}

//------------------------------------------------------------------------------
bool TestShapeAABB3::DoesAABBOverlap( Vec3 aabbMins, Vec3 aabbMaxs ) const
{
	return DoAABB3sOverlap( m_bounds.m_mins, m_bounds.m_maxs, aabbMins, aabbMaxs );
}

Vec3 TestShapeAABB3::GetClosestPoint( Vec3 referencePos ) const
{
	return GetNearestPointOnAABB3( referencePos, m_bounds.m_mins, m_bounds.m_maxs );
}

