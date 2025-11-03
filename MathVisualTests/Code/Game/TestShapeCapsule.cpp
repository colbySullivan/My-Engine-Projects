#include "Game/TestShapeCapsule.hpp"
#include "Engine/Core/Engine.hpp"

TestShapeCapsule::TestShapeCapsule( Vec2 boneStart, Vec2 boneEnd, float radius, Rgba8 color )
	: m_boneStart( boneStart )
	, m_boneEnd( boneEnd )
	, m_color( color )
	, m_radius( radius )
{
	AddVertsForMe( m_lineVerts );
}

TestShapeCapsule::~TestShapeCapsule()
{

}

//-----------------------------------------------------------------------------------------------
void TestShapeCapsule::Update()
{

}

//-----------------------------------------------------------------------------------------------
void TestShapeCapsule::Render() const
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
void TestShapeCapsule::GetClosestPoint( Vec2 pointPos )
{
	m_mousePoint = pointPos;
	m_closestPoint = GetNearestPointOnCapsule2D( pointPos, m_boneStart, m_boneEnd, m_radius );
}

//-----------------------------------------------------------------------------------------------
bool TestShapeCapsule::IsPointInsideMe( Vec2 point ) const
{
	return IsPointInsideCapsule2D( point, m_boneStart, m_boneEnd, m_radius );
}

//-----------------------------------------------------------------------------------------------
void TestShapeCapsule::AddVertsForMe( std::vector<Vertex>& verts ) const
{
	AddVertsForCapsule2D(verts, m_boneStart, m_boneEnd, m_radius, m_color);
}

//-----------------------------------------------------------------------------------------------
void TestShapeCapsule::ChangeColor( Rgba8 newColor )
{
	for ( int i = 0; i < m_lineVerts.size(); ++i )
	{
		m_lineVerts[i].m_color = newColor;
	}
}