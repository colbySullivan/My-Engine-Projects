#include "Game/TestShapeOBB2.hpp"
#include "Engine/Core/Engine.hpp"

TestShapeOBB2::TestShapeOBB2( Vec2 const& center, Vec2 const& iBasisNormal, Vec2 const& halfDimensions, Rgba8 color )
	: m_center( center )
	, m_iBasisNormal( iBasisNormal )
	, m_halfDimensions( halfDimensions )
	, m_color( color )
{
	AddVertsForMe( m_lineVerts );
}

//-----------------------------------------------------------------------------------------------
void TestShapeOBB2::Update()
{

}

//-----------------------------------------------------------------------------------------------
void TestShapeOBB2::Render() const
{
	std::vector<Vertex> closePoint;
	AABB2 pointAABB2( 0.f, 0.f, 1.f, 1.f );
	AddVertsForAABB2D( closePoint, pointAABB2, Rgba8( 255, 0, 0, 255 ) );
	TransformVertexArrayXY3D( closePoint.size(), closePoint.data(), .5f, 0.f, m_closestPoint );
	g_engine->m_render->DrawVertexArray( ( int )m_lineVerts.size(), m_lineVerts.data() );
	g_engine->m_render->DrawVertexArray( closePoint );
}

//-----------------------------------------------------------------------------------------------
void TestShapeOBB2::GetClosestPoint( Vec2 pointPos )
{
	//m_closestPoint = GetNearestPointOnLineSegment2D( pointPos, m_start, m_end );
}

//-----------------------------------------------------------------------------------------------
bool TestShapeOBB2::IsPointInsideMe( Vec2 point ) const
{
	return false;
}

//-----------------------------------------------------------------------------------------------
void TestShapeOBB2::AddVertsForMe( std::vector<Vertex>& verts ) const
{
	//OBB2 orientedBox(Vec2(10.f,10.f), Vec2(10.f,10.f), Vec2(10.f,10.f));
	OBB2 orientedBox(m_center, m_iBasisNormal, m_halfDimensions);
	AddVertsForOBB2D(verts, orientedBox, m_color);
}

