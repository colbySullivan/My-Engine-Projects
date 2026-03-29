#include "Game/TestShapeAABB3.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"

//------------------------------------------------------------------------------
TestShapeAABB3::TestShapeAABB3( const AABB3& bounds, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE */ )
{
	AddVertsForAABB3D( m_aabb3Verts, bounds, color, UVs );
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
bool TestShapeAABB3::DoesSphereOverlap( Vec3 sphereCenter, float radius ) const
{
	return false; // #todo Implement this function
}

