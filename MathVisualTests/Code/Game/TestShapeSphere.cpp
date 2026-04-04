#include "Game/TestShapeSphere.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//------------------------------------------------------------------------------
TestShapeSphere::TestShapeSphere( Vec3 center, float radius, int numSlices, int numStacks, const Rgba8& color )
	: m_center( center )
	, m_radius( radius )
	, m_numSlices( numSlices )
	, m_numStacks( numStacks )
	, m_color( color )
{
	AddVertsForSphere3D( m_sphereVerts, m_center, m_radius, m_numSlices, m_numStacks, m_color );
	m_shapeBlinkTimer = new Timer( 0.5f );
	m_shapeBlinkTimer->Start();
}

//------------------------------------------------------------------------------
TestShapeSphere::~TestShapeSphere()
{
	m_sphereVerts.clear();
}

//------------------------------------------------------------------------------
void TestShapeSphere::Update()
{
	m_sphereVerts.clear();
	AddVertsForSphere3D( m_sphereVerts, m_center, m_radius, m_numSlices, m_numStacks, m_color );
}

//------------------------------------------------------------------------------
void TestShapeSphere::Render() const
{
	IsOverlapped();
	IsCasted();
	g_engine->m_render->m_desiredBlendMode = BlendMode::OPAQUE;
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	g_engine->m_render->BindTexture( nullptr );
	g_engine->m_render->DrawVertexArray( ( int )m_sphereVerts.size(), m_sphereVerts.data() );
}

//------------------------------------------------------------------------------
void TestShapeSphere::RenderWithTexture( Texture* texture ) const
{
	IsOverlapped();
	IsCasted();
	g_engine->m_render->m_desiredBlendMode = BlendMode::OPAQUE;
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	g_engine->m_render->BindTexture( texture );
	g_engine->m_render->DrawVertexArray( ( int )m_sphereVerts.size(), m_sphereVerts.data() );
}

//------------------------------------------------------------------------------
RaycastResult3D TestShapeSphere::RaycastTestShape( Vec3 startPos, Vec3 forwardNormal, float maxDistance ) const
{
	return RaycastVsSphere( startPos, forwardNormal, maxDistance, m_center, m_radius );
}

//------------------------------------------------------------------------------
bool TestShapeSphere::DoesSphereOverlap( Vec3 sphereCenter, float radius ) const
{
	return DoSpheresOverlap( m_center, m_radius, sphereCenter, radius );
}

//------------------------------------------------------------------------------
bool TestShapeSphere::DoesCylinderOverlap( Vec2 cylinderCenter, float cylinderRadius, FloatRange cylinderZRange ) const
{
	return DoSphereCylinderOverlap( m_center, m_radius, Vec3( cylinderCenter.x, cylinderCenter.y, cylinderZRange.m_min ), Vec3( cylinderCenter.x, cylinderCenter.y, cylinderZRange.m_max ), cylinderRadius );
}

//------------------------------------------------------------------------------
bool TestShapeSphere::DoesAABBOverlap( Vec3 aabbMins, Vec3 aabbMaxs ) const
{
	return DoSphereABB3Overlap( m_center, m_radius, aabbMins, aabbMaxs );
}

Vec3 TestShapeSphere::GetClosestPoint( Vec3 referencePos ) const
{
	return GetNearestPointOnSphere( referencePos, m_center, m_radius );
}

