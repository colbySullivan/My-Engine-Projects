#include "Game/TestShapeCylinder.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"

TestShapeCylinder::TestShapeCylinder( const Vec3& start, const Vec3& end, float radius, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/, int numSlices /*= 32 */ )
	: m_radius( radius )
	, m_center( ( start + end ) * 0.5f )
{
	AddVertsForCylinder3D( m_cylinderVerts, start, end, radius, color, UVs, numSlices );
}

TestShapeCylinder::~TestShapeCylinder()
{

}

void TestShapeCylinder::Render() const
{
	g_engine->m_render->m_desiredBlendMode = BlendMode::OPAQUE;
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	g_engine->m_render->BindTexture( nullptr );
	g_engine->m_render->DrawVertexArray( ( int )m_cylinderVerts.size(), m_cylinderVerts.data() );
}

void TestShapeCylinder::RenderWithTexture( Texture* texture ) const
{
	g_engine->m_render->m_desiredBlendMode = BlendMode::OPAQUE;
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	g_engine->m_render->BindTexture( texture );
	g_engine->m_render->DrawVertexArray( ( int )m_cylinderVerts.size(), m_cylinderVerts.data() );
}

