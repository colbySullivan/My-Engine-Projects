#include "App.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/Game.hpp"


App* g_app = nullptr;

App::App()
{
	EngineConfig config;
	config.m_windowConfig.m_clientAspect = 2.0f;
	config.m_windowConfig.m_windowTitle = "Doomenstein";
	g_engine = new Engine( config );

	LoadXmlMap();
	g_app = this;
	m_game = new Game();

	g_UICamera = new Camera();
	g_UICamera->SetOrthoView( Vec2( 0.f, 0.f ),  Vec2(g_gameConfig->GetValue("screenSizeX", 0.f), g_gameConfig->GetValue("screenSizeY", 0.f)));

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
	//if (g_engine->m_input->IsKeyDown(KEYCODE_F8))
	//{
	//	delete m_game;
	//	m_game = nullptr;
	//	m_game = new Game();
	//}
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
	bool isPickingPowerUp = ( m_game && m_game->m_currentMap ) ? m_game->m_currentMap->IsCurrentlyPickingPowerUp() : false;

	if ( !windowHasFocus || devConsoleOpen || inAttractMode || isPickingPowerUp )
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
	g_app->SetIsQuitting();
	return false;
}

//-----------------------------------------------------------------------------------------------
void App::LoadXmlMap()
{
	g_gameConfig = new NamedStrings();
	XmlDocument config;
	XmlError configeResult = config.LoadFile( "Data/Definitions/GameConfig.xml" );
	if ( configeResult == 0 )
	{
		XmlElement* rootElement = config.RootElement();
		g_gameConfig->PopulateFromXmlElementAttributes( *rootElement );
	}
}