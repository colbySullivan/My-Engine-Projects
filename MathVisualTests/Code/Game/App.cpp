#include "App.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/Game.hpp"
#include "Game/GameNearestPoint.hpp"
#include "Game/GameRaycastVsDiscs.hpp"


App* g_app = nullptr;

App::App()
{
	EngineConfig config;
	config.m_windowConfig.m_clientAspect = 2.0f;
	config.m_windowConfig.m_windowTitle = "MathVisualTests";
	g_engine = new Engine( config );

	g_gameMode = GAMEMODE_NEAREST_POINT;

	m_game = new GameNearestPoint( this );
	g_theGame = m_game;

	m_game->Startup();
}

//-----------------------------------------------------------------------------------------------

App::~App()
{
	delete m_game;
	m_game = nullptr;

	delete g_engine;
	g_engine = nullptr;
}
//-----------------------------------------------------------------------------------------------

void App::RunFrame()
{
	float timeNow = (float)GetCurrentTimeSeconds();
	float deltaSeconds = timeNow - m_lastFrameTime;
	m_lastFrameTime = timeNow;

	g_engine->BeginFrame();
	Update(deltaSeconds);
	Render();
	g_engine->EndFrame();
}

//-----------------------------------------------------------------------------------------------
void App::Update( float deltaSeconds )
{
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

	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F8 ) )
	{
		m_game->Shutdown();
		delete m_game;
		m_game = CreateNewGameOfType( g_gameMode );
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

