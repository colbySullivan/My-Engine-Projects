#include "Game/TestShapeLine.hpp"
#include "Engine/Core/Engine.hpp"

TestShapeLine::TestShapeLine( Vec2 start, Vec2 end, Vec2 thickness, Rgba8 color )
	: m_start( start )
	, m_end( end )
	, m_thickness( thickness )
	, m_color( color )
{
	AddVertsForMe(m_lineVerts);
}

//-----------------------------------------------------------------------------------------------
void TestShapeLine::Update()
{
	
}

//-----------------------------------------------------------------------------------------------
void TestShapeLine::Render() const
{
	std::vector<Vertex> closePoint;
	AddVertsForDisc2D( closePoint, Vec2( 0.f, 0.f ), 1.0f, Rgba8( 255, 215, 0, 255 ) );
	TransformVertexArrayXY3D( static_cast<int>(closePoint.size()), closePoint.data(), .5f, 0.f, m_closestPoint );
	g_engine->m_render->DrawVertexArray( ( int )m_lineVerts.size(), m_lineVerts.data() );
	g_engine->m_render->DrawVertexArray( closePoint );
}

//-----------------------------------------------------------------------------------------------
void TestShapeLine::GetClosestPoint( Vec2 pointPos )
{
	m_closestPoint = GetNearestPointOnLineSegment2D( pointPos, m_start, m_end );
}

//-----------------------------------------------------------------------------------------------
bool TestShapeLine::IsPointInsideMe( [[maybe_unused]] Vec2 point ) const
{
	return false;
}

//-----------------------------------------------------------------------------------------------
void TestShapeLine::AddVertsForMe( std::vector<Vertex>& verts ) const
{
	AddVertsForLineSegment2D(verts, m_start, m_end, m_thickness, m_color);
}

//-----------------------------------------------------------------------------------------------
void TestShapeLine::ChangeColor(Rgba8 newColor)
{
	for (int i = 0; i < m_lineVerts.size(); ++i)
	{
		m_lineVerts[i].m_color = newColor;
	}
}