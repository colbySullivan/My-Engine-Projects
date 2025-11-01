#include "Game/TestShapeAABB2.hpp"
#include "Engine/Core/Engine.hpp"

TestShapeAABB2::TestShapeAABB2( Vec2 mins, Vec2 maxs, Rgba8 color )
	: m_mins( mins )
	, m_maxs( maxs )
	, m_color( color )
{
	m_aabb2Box = new AABB2(m_mins, m_maxs);
	AddVertsForMe( m_lineVerts );
}

TestShapeAABB2::~TestShapeAABB2()
{
	delete m_aabb2Box;
}

//-----------------------------------------------------------------------------------------------
void TestShapeAABB2::Update()
{

}

//-----------------------------------------------------------------------------------------------
void TestShapeAABB2::Render() const
{
	std::vector<Vertex> closePoint;
	AddVertsForDisc2D( closePoint, Vec2( 0.f, 0.f ), 1.0f, Rgba8( 255, 215, 0, 255 ) );
	TransformVertexArrayXY3D( static_cast<int>(closePoint.size()), closePoint.data(), .5f, 0.f, m_closestPoint );
	g_engine->m_render->DrawVertexArray( static_cast<int>(m_lineVerts.size()), m_lineVerts.data() );
	g_engine->m_render->DrawVertexArray( closePoint );
}

//-----------------------------------------------------------------------------------------------
void TestShapeAABB2::GetClosestPoint( Vec2 pointPos )
{
	//m_closestPoint = GetNearestPointOnDisc2D( pointPos, m_center, m_discRadius );
	m_closestPoint = GetNearestPointOnAABB2D(pointPos, *m_aabb2Box);
}

//-----------------------------------------------------------------------------------------------
bool TestShapeAABB2::IsPointInsideMe( Vec2 point ) const
{
	return IsPointInsideAABB2D(point, *m_aabb2Box);
}

//-----------------------------------------------------------------------------------------------
void TestShapeAABB2::AddVertsForMe( std::vector<Vertex>& verts ) const
{
	AddVertsForAABB2D(verts, *m_aabb2Box, m_color);
}

//-----------------------------------------------------------------------------------------------
void TestShapeAABB2::ChangeColor( Rgba8 newColor )
{
	for ( int i = 0; i < m_lineVerts.size(); ++i )
	{
		m_lineVerts[i].m_color = newColor;
	}
}