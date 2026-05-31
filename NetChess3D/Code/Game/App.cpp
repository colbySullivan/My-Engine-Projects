#include "App.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Game/Game.hpp"


App* g_app = nullptr;

App::App()
{
	EngineConfig config;
	config.m_windowConfig.m_clientAspect = 2.0f;
	config.m_windowConfig.m_windowTitle = "NetChess3D";
	g_engine = new Engine( config );

	g_app = this;
	m_game = new Game();

	g_UICamera = new Camera();
	g_UICamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y) );

	DebugRenderConfig debugConfig;
	DebugRenderSystemStartup( debugConfig );

	SubscribeEventCallbackFunction( "Quit", App::Event_Quit );
	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "ToggleDebug", Command_DebugRenderToggle );
	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "ClearDebug", Command_DebugRenderClear );
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
	DebugRenderBeginFrame();
	Update();
	Render();
	DebugRenderEndFrame();
	g_engine->EndFrame();
}
//-----------------------------------------------------------------------------------------------

void App::Update()
{
	m_game->Update();
	UpdateCursorMode();
	if (g_engine->m_input->IsKeyDown(KEYCODE_F8))
	{
		delete m_game;
		m_game = nullptr;
		m_game = new Game();
	}
}
//-----------------------------------------------------------------------------------------------

void App::Render() const
{
	m_game->Render();
	g_engine->m_render->BeginCamera( *g_UICamera );
	g_engine->m_console->Render( AABB2( g_UICamera->GetOrthoBottomLeft(), g_UICamera->GetOrthoTopRight() ), *m_game->g_testFont, 1.f );
	g_engine->m_render->EndCamera( *g_UICamera );
}

//-----------------------------------------------------------------------------------------------
void App::UpdateCursorMode()
{
	HWND hwnd = ( HWND )g_engine->m_window->GetHwnd();

	bool windowHasFocus = ( GetForegroundWindow() == hwnd );
	bool devConsoleOpen = g_engine->m_console && ( g_engine->m_console->GetMode() == OPEN_FULL );
	bool inAttractMode = m_game && ( m_game->m_currentGameState == GAMESTATE_ATTRACT );

	if ( !windowHasFocus || devConsoleOpen || inAttractMode )
	{
		g_engine->m_input->SetCursorMode( CursorMode::POINTER );
	}
	else
	{
		g_engine->m_input->SetCursorMode( CursorMode::FPS );
	}
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

//------------------------------------------------------------------------------
bool App::Event_Quit( [[maybe_unused]] EventArgs& args )
{
	g_theApp->SetIsQuitting();
	return false;
}
