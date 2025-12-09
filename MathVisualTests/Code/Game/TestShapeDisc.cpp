#include "Game/TestShapeDisc.hpp"
#include "Engine/Core/Engine.hpp"

TestShapeDisc::TestShapeDisc(Vec2 center, float radius, Rgba8 color)
	: m_center(center)
	, m_discRadius(radius)
	, m_color(color)
{
	AddVertsForMe(m_lineVerts);
}

//-----------------------------------------------------------------------------------------------
void TestShapeDisc::Update()
{

}

//-----------------------------------------------------------------------------------------------
void TestShapeDisc::Render() const
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
void TestShapeDisc::GetClosestPoint(Vec2 pointPos)
{
	m_mousePoint = pointPos;
	m_closestPoint = GetNearestPointOnDisc2D(pointPos, m_center, m_discRadius);
}

//-----------------------------------------------------------------------------------------------
bool TestShapeDisc::IsPointInsideMe(Vec2 point) const
{
	return IsPointInsideDisc2D(point, m_center, m_discRadius);
}

//-----------------------------------------------------------------------------------------------
void TestShapeDisc::AddVertsForMe(std::vector<Vertex>& verts) const
{
	AddVertsForDisc2D(verts, m_center, m_discRadius, m_color);
}

//-----------------------------------------------------------------------------------------------
void TestShapeDisc::ChangeColor(Rgba8 newColor)
{
	for (int i = 0; i <  static_cast<int>(m_lineVerts.size()); ++i)
	{
		m_lineVerts[i].m_color = newColor;
	}
}