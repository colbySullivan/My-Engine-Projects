#include "Game/GameNearestPoint.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Camera.hpp"

//------------------------------------------------------------------------------
GameNearestPoint::GameNearestPoint(App* app)
	: Game(app)
{
	AddShapeVerts();
}

GameNearestPoint::~GameNearestPoint()
{

}

//-----------------------------------------------------------------------------------------------
void GameNearestPoint::Update( float deltaTime )
{	
	
}

//-----------------------------------------------------------------------------------------------
void GameNearestPoint::Render() const
{
	Rgba8 backgroundColor = Rgba8( static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 255.f ) ); // Suppresses error with conversion

	g_engine->m_render->ClearScreen( backgroundColor );

	g_engine->m_render->DrawVertexArray( ( int )m_line->m_lineVerts.size(), m_line->m_lineVerts.data() );
}

//-----------------------------------------------------------------------------------------------
void GameNearestPoint::AddShapeVerts()
{
	m_line = new TestShapeLine( Vec2( 0.f, 0.f ), Vec2( 5.f, 2.f ), Vec2( 0.1f, 0.f ), Rgba8( 255, 255, 255, 255 ) );
	AddVertsForLineSegment2D(m_line->m_lineVerts, m_line->m_start, m_line->m_end, m_line->m_thickness, m_line->m_color);
}
