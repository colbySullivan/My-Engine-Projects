#include "Game/TestShapeSphere.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"

//------------------------------------------------------------------------------
TestShapeSphere::TestShapeSphere( Vec3 center, float radius, int numSlices, int numStacks, const Rgba8& color )
	: m_center( center )
	, m_radius( radius )
{
	AddVertsForSphere3D( m_sphereVerts, m_center, m_radius, numSlices, numStacks, color );
}

//------------------------------------------------------------------------------
TestShapeSphere::~TestShapeSphere()
{

}

//------------------------------------------------------------------------------
void TestShapeSphere::Render() const
{
	g_engine->m_render->m_desiredBlendMode = BlendMode::OPAQUE;
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	g_engine->m_render->BindTexture( nullptr );
	g_engine->m_render->DrawVertexArray( ( int )m_sphereVerts.size(), m_sphereVerts.data() );
}

