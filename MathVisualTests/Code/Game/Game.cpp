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
#include "Game/GameRaycastVsDiscs.hpp"
#include "Game/GameNearestPoint.hpp"
#include "Game/App.hpp"

RandomNumberGenerator g_rng;

//-----------------------------------------------------------------------------------------------
Game::Game([[maybe_unused]] App* m_app)
	:m_app( g_theApp )
{
	m_worldCamera = new Camera;
	m_screenCamera = new Camera;
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
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_NONE;
	g_testFont = g_engine->m_render->CreateOrGetBitmapFont( "Data/Fonts/SquirrelFixedFont" );
}

//-----------------------------------------------------------------------------------------------
void Game::Shutdown()
{

}

//-----------------------------------------------------------------------------------------------
// Input Handling
//-----------------------------------------------------------------------------------------------
void Game::UpdateKeyboardInput()
{
	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_ESC ) )
	{
		m_isQuitting = true;
		return;
	}

	m_isSlowMo = g_engine->m_input->IsKeyDown('T');  // Slows simulation time to 1/10th the normal rate

	if (g_engine->m_input->WasKeyJustPressed('P')) // Pauses game
	{
		m_isPaused = !m_isPaused; // Switch pause
	}
	if (g_engine->m_input->WasKeyJustPressed('O')) // Runs a single unpaused Update (simulation step) and then pauses.
	{
		m_isPaused = true;
		m_pauseAfterNextUpdate = true; // Consumed to false after one run of update
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
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
	//m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );
}


void Game::RenderGameText( GameType type ) const
{
	char powerUpText[128];
	switch ( type )
	{
		case GAMEMODE_NEAREST_POINT:				snprintf(powerUpText, sizeof(powerUpText), "Mode (F6/F7 for prev/next): Nearest point (2D)");				break;
		case GAMEMODE_RAYCAST_VS_DISCS:				snprintf(powerUpText, sizeof(powerUpText), "Mode (F6/F7 for prev/next): Raycast vs Discs (2D)");			break;
		case GAMEMODE_RAYCAST_VS_LINE_SEGMENTS:     snprintf(powerUpText, sizeof(powerUpText), "Mode (F6/F7 for prev/next): Raycast vs Line Segments (2D)");	break;
		case GAMEMODE_RAYCAST_VS_AABB2:				snprintf(powerUpText, sizeof(powerUpText), "Mode (F6/F7 for prev/next): Raycast vs AABB2 Segments (2D)");	break;
		case GAMEMODE_2D_CURVES:					snprintf(powerUpText, sizeof(powerUpText), "Mode (F6/F7 for prev/next): Easing, Curve, Spline (2D)");		break;
	}
	RenderText( powerUpText, Vec2( 1.f, 95.f ), 2.f, Rgba8( 255, 215, 0, 255 ) );
	RenderText( "F8 to randomize; WASD/Arrows = move reference point, hold T = slow", Vec2( 1.f, 93.f ), 2.f, Rgba8( 100, 120, 180, 255 ) );
}
