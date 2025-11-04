#include "Game/GameRaycastVsDiscs.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Camera.hpp"

//------------------------------------------------------------------------------
GameRaycastVsDiscs::GameRaycastVsDiscs( App* app )
	: Game( app )
{
}

GameRaycastVsDiscs::~GameRaycastVsDiscs()
{
	for ( int Index = 0; Index < m_testShapes.size(); Index++ )
	{
		if ( m_testShapes[Index] != nullptr )
		{
			delete m_testShapes[Index];
			m_testShapes[Index] = nullptr;
		}
	}
	m_testShapes.clear();
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

	Game::RenderGameText( GAMEMODE_RAYCAST_VS_DISCS );

	RenderShapes();

	g_engine->m_render->EndCamera( *m_worldCamera );
}

//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::AddShapeVerts()
{
	TestShape* infiniteLine = new TestShapeLine( Vec2( 10.f, 10.f ), Vec2( 190.f, 90.f ), Vec2( 0.5f, 0.5f ), Rgba8( 255, 0, 0, 255 ) );
	m_testShapes.push_back( infiniteLine );
}


//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::RenderShapes() const
{
	for ( int Index = 0; Index < m_testShapes.size(); Index++ )
	{
		TestShape* shape = m_testShapes[Index];
		if ( shape != nullptr )
		{
			shape->Render();
		}
	}
}

