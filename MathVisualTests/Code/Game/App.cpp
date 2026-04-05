#include "App.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Game/Game.hpp"
#include "Game/GameNearestPoint.hpp"
#include "Game/GameRaycastVsDiscs.hpp"
#include "Game/GameRaycastVsLineSegments.hpp"
#include "Game/GameRaycastVsAABB2.hpp"
#include "Game/TestShapes3D.hpp"


App* g_app = nullptr;

App::App()
{
	EngineConfig config;
	config.m_windowConfig.m_clientAspect = 2.0f;
	config.m_windowConfig.m_windowTitle = "MathVisualTests";
	g_engine = new Engine( config );

	DebugRenderConfig debugConfig;
	DebugRenderSystemStartup( debugConfig );

	g_gameMode = GAMEMODE_TESTSHAPES_3D;
	m_game = new TestShapes3D( this ); // #TODO Revert this to GAMEMODE_NEAREST_POINT

	m_game->Startup();
}

//-----------------------------------------------------------------------------------------------

App::~App()
{
	DebugRenderSystemShutdown();
	delete m_game;
	m_game = nullptr;

	delete g_engine;
	g_engine = nullptr;
}
//-----------------------------------------------------------------------------------------------

void App::RunFrame()
{
	g_engine->BeginFrame();
	
	Update((float)g_engine->m_systemClock->GetDeltaSeconds());
	Render();
	if ( g_gameMode == GAMEMODE_TESTSHAPES_3D )
	{
		DebugRenderEndFrame();
	}
	g_engine->EndFrame();
}

//-----------------------------------------------------------------------------------------------
void App::Update( float deltaSeconds )
{
	UpdateCursorMode();
	// Handle mode switching in App
	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F7 ) )
	{
		g_gameMode = static_cast< GameType >( ( g_gameMode + 1 ) % GAME_NUM_TYPES );

		m_game->Shutdown();
		delete m_game;
		m_game = CreateNewGameOfType( g_gameMode );
		m_game->Startup();
		return;
	}

	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F6 ) )
	{
		g_gameMode = static_cast< GameType >( ( g_gameMode + (GAME_NUM_TYPES - 1) ) % GAME_NUM_TYPES );

		m_game->Shutdown();
		delete m_game;
		m_game = CreateNewGameOfType( g_gameMode );
		m_game->Startup();
		return;
	}

	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F8 ) )
	{
		m_game->Shutdown();
		/*delete m_game;
		m_game = CreateNewGameOfType( g_gameMode );*/
		m_game->Startup();
		return;
	}
	m_game->Update( deltaSeconds );
	
}

Game* App::CreateNewGameOfType( GameType type )
{
	Game* newGame = nullptr;
	switch ( type )
	{
	case GAMEMODE_NEAREST_POINT:    newGame = new GameNearestPoint( this );    break;
	case GAMEMODE_RAYCAST_VS_DISCS: newGame = new GameRaycastVsDiscs( this ); break;
	case GAMEMODE_RAYCAST_VS_LINE_SEGMENTS: newGame = new GameRaycastVsLineSegments( this ); break;
	case GAMEMODE_RAYCAST_VS_AABB2: newGame = new GameRaycastVsAABB2( this ); break;
	case GAMEMODE_TESTSHAPES_3D: newGame = new TestShapes3D( this ); break;
	}
	return newGame;
}
//-----------------------------------------------------------------------------------------------

void App::Render() const
{
	m_game->Render();
}
//-----------------------------------------------------------------------------------------------

void App::SetIsQuitting()
{
	m_game->m_isQuitting = true;
}
//-----------------------------------------------------------------------------------------------

bool App::IsQuitting() const
{
	return m_game->m_isQuitting;
}

void App::UpdateCursorMode()
{
	HWND hwnd = ( HWND )g_engine->m_window->GetHwnd();

	bool windowHasFocus = ( GetForegroundWindow() == hwnd );
	bool devConsoleOpen = g_engine->m_console && ( g_engine->m_console->GetMode() == OPEN_FULL );
	bool in3DMode = m_game && ( g_gameMode != GAMEMODE_TESTSHAPES_3D );

	if ( !windowHasFocus || devConsoleOpen || in3DMode )
	{
		g_engine->m_input->SetCursorMode( CursorMode::POINTER );
	}
	else
	{
		g_engine->m_input->SetCursorMode( CursorMode::FPS );
	}
}