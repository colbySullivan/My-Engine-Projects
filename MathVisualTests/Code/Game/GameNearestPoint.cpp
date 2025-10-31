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
	Game::UpdateCameras(deltaTime);
	UpdatePointPosition(deltaTime);
	m_line->GetClosestPoint(m_pointPos);
}

//-----------------------------------------------------------------------------------------------
void GameNearestPoint::Render() const
{
	g_engine->m_render->BeginCamera( *m_worldCamera );
	Rgba8 backgroundColor = Rgba8( static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 255.f ) ); // Suppresses error with conversion

	g_engine->m_render->ClearScreen( backgroundColor );
	g_engine->m_render->DrawVertexArray( ( int )m_line->m_lineVerts.size(), m_line->m_lineVerts.data() );

	Vertex tempPointWorldVerts[6];

	for ( int vertIndex = 0; vertIndex < m_pointVerts.size(); ++vertIndex )
	{
		tempPointWorldVerts[vertIndex] = m_pointVerts[vertIndex];
	}

	TransformVertexArrayXY3D(m_pointVerts.size(), tempPointWorldVerts, .5f, 0.f, m_pointPos);
	g_engine->m_render->DrawVertexArray(6, tempPointWorldVerts);

	m_line->Render();

	g_engine->m_render->EndCamera( *m_worldCamera );
}

//-----------------------------------------------------------------------------------------------
void GameNearestPoint::AddShapeVerts()
{
	// LineTestShape
	m_line = new TestShapeLine( Vec2( 10.f, 10.f ), Vec2( 50.f, 60.f ), Vec2( 0.5f, 0.5f ), Rgba8( 255, 255, 255, 255 ) );
	AddVertsForLineSegment2D(m_line->m_lineVerts, m_line->m_start, m_line->m_end, m_line->m_thickness, m_line->m_color);

	// Player moving point shape
	AABB2 pointAABB2( 0.f, 0.f, 1.f, 1.f );
	AddVertsForAABB2D( m_pointVerts, pointAABB2, Rgba8( 255, 255, 255, 255 ) );
}

//-----------------------------------------------------------------------------------------------
void GameNearestPoint::UpdatePointPosition( float deltaTime )
{

	Vec2 moveDirection = Vec2(0.f,0.f);
	if ( g_engine->m_input->IsKeyDown( 'W' ) ) {
		moveDirection.y += 1.f;
	}
	if ( g_engine->m_input->IsKeyDown( 'S' ) ) {
		moveDirection.y = -1.f;
	}
	if ( g_engine->m_input->IsKeyDown( 'A' ) ) {
		moveDirection.x = -1.f;
	}
	if ( g_engine->m_input->IsKeyDown( 'D' ) ) {
		moveDirection.x = 1.f;
	}

	if ( moveDirection.GetLengthSquared() > 0.f ) {
		moveDirection.Normalize();
		m_pointPos += moveDirection * m_speed * deltaTime;
	}
}
