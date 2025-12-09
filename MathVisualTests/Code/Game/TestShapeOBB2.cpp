#include "Game/TestShapeOBB2.hpp"
#include "Engine/Core/Engine.hpp"

TestShapeOBB2::TestShapeOBB2( Vec2 const& center, Vec2 const& iBasisNormal, Vec2 const& halfDimensions, Rgba8 color )
	: m_center( center )
	, m_iBasisNormal( iBasisNormal )
	, m_halfDimensions( halfDimensions )
	, m_color( color )
{
	m_orientedBox = new OBB2(m_center, m_iBasisNormal, m_halfDimensions);
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
	std::vector<Vertex> lineToPoints;
	AddVertsForDisc2D( closePoint, Vec2( 0.f, 0.f ), 1.0f, Rgba8( 255, 215, 0, 255 ) );
	TransformVertexArrayXY3D( static_cast< int >( closePoint.size() ), closePoint.data(), .5f, 0.f, m_closestPoint );
	AddVertsForLineSegment2D( lineToPoints, m_mousePoint, m_closestPoint, Vec2( 0.1f, 0.1f ), Rgba8( 255, 255, 255, 100 ) );
	g_engine->m_render->DrawVertexArray( lineToPoints );
	g_engine->m_render->DrawVertexArray( ( int )m_lineVerts.size(), m_lineVerts.data() );
	g_engine->m_render->DrawVertexArray( closePoint );
}

//-----------------------------------------------------------------------------------------------
void TestShapeOBB2::GetClosestPoint( Vec2 pointPos )
{
	m_mousePoint = pointPos;
	m_closestPoint = GetNearestPointOnOBB2D( pointPos, *m_orientedBox);
}

//-----------------------------------------------------------------------------------------------
bool TestShapeOBB2::IsPointInsideMe( Vec2 point ) const
{
	return IsPointInsideOBB2D(point, *m_orientedBox);
}

//-----------------------------------------------------------------------------------------------
void TestShapeOBB2::AddVertsForMe( std::vector<Vertex>& verts ) const
{
	AddVertsForOBB2D(verts, *m_orientedBox, m_color);
}

//-----------------------------------------------------------------------------------------------
void TestShapeOBB2::ChangeColor(Rgba8 newColor)
{
	for (int i = 0; i <  static_cast<int>(m_lineVerts.size()); ++i)
	{
		m_lineVerts[i].m_color = newColor;
	}
}
