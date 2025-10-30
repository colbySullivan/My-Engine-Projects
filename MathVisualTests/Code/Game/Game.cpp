#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"
#include "Game/Entity.hpp"

RandomNumberGenerator g_rng;

//-----------------------------------------------------------------------------------------------
Game::Game([[maybe_unused]] App* m_app)
	:m_app( g_theApp )
{
	m_worldCamera = new Camera;
	m_screenCamera = new Camera;
	AddVertsForLineSegment2D(m_lineVerts, Vec2(0.f,0.f), Vec2(1000.f,1000.f), Vec2(1.0f,1.0f), Rgba8(255, 255, 255, 255));
}

//-----------------------------------------------------------------------------------------------
Game::~Game()
{
	delete m_worldCamera;
	delete m_screenCamera;
	m_worldCamera = nullptr;
	m_screenCamera = nullptr;
}

//-----------------------------------------------------------------------------------------------
// Game Loop
//-----------------------------------------------------------------------------------------------
void Game::Startup()
{
	Vec2 worldCenter(WORLD_SIZE_X * 0.5f, WORLD_SIZE_Y * 0.5f);
	m_isPaused = false;
}

//-----------------------------------------------------------------------------------------------
void Game::Update(float deltaSeconds)
{
	XboxController const& controller = g_engine->m_input->GetController( 0 );

	UpdateCameras( deltaSeconds );
	UpdateKeyboardInput( controller );

	if ( m_isSlowMo ) // T pressed
	{
		deltaSeconds = 1.f / 600.f; // Run at 1/10th the speed
	}

	if ( !m_isPaused || m_pauseAfterNextUpdate )
	{
		m_pauseAfterNextUpdate = false; // Reset run token for simulation step
	}
}

//-----------------------------------------------------------------------------------------------
void Game::Render() const
{
	Rgba8 backgroundColor = Rgba8(static_cast<unsigned char>(0.f), static_cast<unsigned char>(0.f), static_cast<unsigned char>(0.f), static_cast<unsigned char>(255.f)); // Suppresses error with conversion

	g_engine->m_render->ClearScreen(backgroundColor);

	g_engine->m_render->BeginCamera( *m_screenCamera );
	g_engine->m_render->DrawVertexArray( ( int )m_lineVerts.size(), m_lineVerts.data() );
	g_engine->m_render->EndCamera( *m_screenCamera );

}

//-----------------------------------------------------------------------------------------------
void Game::Shutdown()
{

}

//-----------------------------------------------------------------------------------------------
// Input Handling
//-----------------------------------------------------------------------------------------------
void Game::UpdateKeyboardInput( XboxController const& controller )
{
	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_ESC ) || controller.WasButtonJustPressed( XboxButtonID::BACK ) )
	{
		m_isQuitting = true;
		return;
	}

	if ( ( g_engine->m_input->WasKeyJustPressed( KEYCODE_ESC ) || controller.WasButtonJustPressed( XboxButtonID::BACK ) ))
	{
		
	}

	m_isSlowMo = g_engine->m_input->IsKeyDown('T');  // Slows simulation time to 1/10th the normal rate

	if (g_engine->m_input->WasKeyJustPressed('P') || controller.WasButtonJustPressed(XboxButtonID::START)) // Pauses game
	{
		m_isPaused = !m_isPaused; // Switch pause
	}
	if (g_engine->m_input->WasKeyJustPressed('O')) // Runs a single unpaused Update (simulation step) and then pauses.
	{
		m_isPaused = true;
		m_pauseAfterNextUpdate = true; // Consumed to false after one run of update
	}
	if ( g_engine->m_input->WasKeyJustPressed( ' ' ) || g_engine->m_input->WasKeyJustPressed( 'N' ) || controller.WasButtonJustPressed( XboxButtonID::START ) )
	{
		Startup();
	}

	if (g_engine->m_input->WasKeyJustPressed('I'))
	{

	}

	if (g_engine->m_input->WasKeyJustPressed(KEYCODE_F1))
	{
		g_drawDebug = !g_drawDebug;
	}

	g_engine->m_input->EndFrame();
}


//-----------------------------------------------------------------------------------------------
void Game::RenderText(const char text[] , Vec2 pos, float height, Rgba8 color) const
{
	std::vector<Vertex> textVerts;
	AddVertsForTextTriangles2D( textVerts, text, pos, height, color, 1.f );
	g_engine->m_render->DrawVertexArray( ( int )textVerts.size(), textVerts.data() );
}


//-----------------------------------------------------------------------------------------------
void Game::UpdateCameras( [[maybe_unused]] float deltaSeconds )
{
	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );
}