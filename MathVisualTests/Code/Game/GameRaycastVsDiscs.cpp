#include "Game/GameRaycastVsDiscs.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Camera.hpp"

//------------------------------------------------------------------------------
GameRaycastVsDiscs::GameRaycastVsDiscs( App* app )
	: Game( app )
{
	// TODO randomize this
	m_tailPos = Vec2( 0.f, 0.f );
	m_tipPos = Vec2( 150.f, 75.f );
}

GameRaycastVsDiscs::~GameRaycastVsDiscs()
{
	
}

//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::Startup()
{
	Game::Startup();
	AddShapeVerts();
}

//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::Update( [[maybe_unused]] float deltaSeconds )
{
	UpdateCameras( deltaSeconds );
	if ( m_isSlowMo ) // T pressed
	{
		deltaSeconds = 1.f / 600.f; // Run at 1/10th the speed
	}

	UpdateCameras( deltaSeconds );
	Game::UpdateKeyboardInput();

	if ( !m_isPaused || m_pauseAfterNextUpdate )
	{
		m_pauseAfterNextUpdate = false;
	}
	//UpdateLine();
	UpdateKeyboardPoints();
	AddShapeVerts();
}

//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::Render() const
{
	Rgba8 backgroundColor = Rgba8( static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 255.f ) ); // Suppresses error with conversion

	g_engine->m_render->ClearScreen( backgroundColor );

	g_engine->m_render->BeginCamera( *m_worldCamera );

	std::vector<Vertex> ring;
	AddVertsForDisc2D( ring, Vec2( 150.f, 75.f ), 20.f, Rgba8( 0, 0, 255, 200 ) );
	g_engine->m_render->DrawVertexArray( ring );
	g_engine->m_render->DrawVertexArray( m_lineVerts );

	Game::RenderGameText( GAMEMODE_RAYCAST_VS_DISCS );

	g_engine->m_render->EndCamera( *m_worldCamera );
}

//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::AddShapeVerts()
{
	m_lineVerts.clear();
	//AddVertsForArrow2D( m_lineVerts, Vec2( 0.f, 0.f ), Vec2( 150.f, 75.f ), 20.f, 1.f, Rgba8( 255, 255, 255 ) );
	AddVertsForArrow2D( m_lineVerts, m_tailPos, m_tipPos, 20.f, 1.f, Rgba8( 255, 255, 255 ) );

}

//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::UpdateLine()
{
	// TODO figure out how to use mouse pos to transform
	TransformVertexArrayXY3D( static_cast< int >( m_lineVerts.size() ), m_lineVerts.data(), 1.f, 0.f, Vec2(1.f,1.f) );
}

//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::UpdateKeyboardPoints()
{
	// Tail point
	if ( g_engine->m_input->IsKeyDown( 'W' ) )
	{
		m_tailPos = m_tailPos + Vec2(0.f,1.f);
	}
	if ( g_engine->m_input->IsKeyDown( 'A' ) )
	{
		m_tailPos = m_tailPos + Vec2( -1.f, 0.f );
	}
	if ( g_engine->m_input->IsKeyDown( 'S' ) )
	{
		m_tailPos = m_tailPos + Vec2( 0.f, -1.f );
	}
	if ( g_engine->m_input->IsKeyDown( 'D' ) )
	{
		m_tailPos = m_tailPos + Vec2( 1.f, 0.f );
	}

	// Head point
	if ( g_engine->m_input->IsKeyDown( 'I' ) )
	{
		m_tipPos = m_tipPos + Vec2( 0.f, 1.f );
	}
	if ( g_engine->m_input->IsKeyDown( 'J' ) )
	{
		m_tipPos = m_tipPos + Vec2( -1.f, 0.f );
	}
	if ( g_engine->m_input->IsKeyDown( 'K' ) )
	{
		m_tipPos = m_tipPos + Vec2( 0.f, -1.f );
	}
	if ( g_engine->m_input->IsKeyDown( 'L' ) )
	{
		m_tipPos = m_tipPos + Vec2( 1.f, 0.f );
	}
}

