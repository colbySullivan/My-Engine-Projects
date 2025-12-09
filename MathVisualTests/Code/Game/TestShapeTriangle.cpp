#include "Game/TestShapeTriangle.hpp"
#include "Engine/Core/Engine.hpp"

TestShapeTriangle::TestShapeTriangle( Vec2 ccw0, Vec2 ccw1, Vec2 ccw2, Rgba8 color )
	: m_aPoint( ccw0 )
	, m_bPoint( ccw1 )
	, m_cPoint( ccw2 )
	, m_color( color )
{
	AddVertsForMe( m_lineVerts );
}

//-----------------------------------------------------------------------------------------------
void TestShapeTriangle::Update()
{

}

//-----------------------------------------------------------------------------------------------
void TestShapeTriangle::Render() const
{
	std::vector<Vertex> closePoint;
	std::vector<Vertex> lineToPoints;
	AddVertsForDisc2D( closePoint, Vec2( 0.f, 0.f ), 1.0f, Rgba8( 255, 215, 0, 255 ) );
	TransformVertexArrayXY3D( static_cast< int >( closePoint.size() ), closePoint.data(), .5f, 0.f, m_closestPoint );
	AddVertsForLineSegment2D(lineToPoints, m_mousePoint, m_closestPoint, Vec2(0.1f,0.1f), Rgba8(255, 255, 255, 100));
	g_engine->m_render->DrawVertexArray( lineToPoints );
	g_engine->m_render->DrawVertexArray( ( int )m_lineVerts.size(), m_lineVerts.data() );
	g_engine->m_render->DrawVertexArray( closePoint );
}

//-----------------------------------------------------------------------------------------------
void TestShapeTriangle::GetClosestPoint( Vec2 pointPos )
{
	m_mousePoint = pointPos;
	m_closestPoint = GetNearestPointOnTriangle2D(pointPos, m_aPoint, m_bPoint, m_cPoint);
}

//-----------------------------------------------------------------------------------------------
bool TestShapeTriangle::IsPointInsideMe( Vec2 point ) const
{
	return IsPointInsideTriangle2D(point, m_aPoint, m_bPoint, m_cPoint);
}

//-----------------------------------------------------------------------------------------------
void TestShapeTriangle::AddVertsForMe( std::vector<Vertex>& verts ) const
{
	AddVertsForTriangle2D( verts, m_aPoint, m_bPoint, m_cPoint, m_color );
}

//-----------------------------------------------------------------------------------------------
void TestShapeTriangle::ChangeColor(Rgba8 newColor)
{
	for (int i = 0; i <  static_cast<int>(m_lineVerts.size()); ++i)
	{
		m_lineVerts[i].m_color = newColor;
	}
}

