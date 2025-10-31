#include "Game/TestShapeLine.hpp"
#include "Engine/Core/Engine.hpp"

TestShapeLine::TestShapeLine( Vec2 start, Vec2 end, Vec2 thickness, Rgba8 color )
	: m_start( start )
	, m_end( end )
	, m_thickness( thickness )
	, m_color( color )
{

}

//-----------------------------------------------------------------------------------------------
void TestShapeLine::Update()
{
	
}

//-----------------------------------------------------------------------------------------------
void TestShapeLine::Render() const
{
	std::vector<Vertex> closePoint;
	AABB2 pointAABB2( 0.f, 0.f, 1.f, 1.f );
	AddVertsForAABB2D( closePoint, pointAABB2, Rgba8( 255, 0, 0, 255 ) );
	TransformVertexArrayXY3D( closePoint.size(), closePoint.data(), .5f, 0.f, m_closestPoint );
	g_engine->m_render->DrawVertexArray(closePoint);
}

//-----------------------------------------------------------------------------------------------
Vec2 TestShapeLine::GetClosestPoint( Vec2 pointPos )
{

	m_closestPoint = GetNearestPointOnLineSegment2D( pointPos, m_start, m_end );
	return Vec2(0.f,0.f);
	
}

