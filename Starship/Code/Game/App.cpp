#include "App.hpp"
#include "PlayerShip.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/Game.hpp"


App* g_app = nullptr;

App::App()
{
	EngineConfig config;
	config.m_windowConfig.m_clientAspect = 2.0f;
	config.m_windowConfig.m_windowTitle = "Starship Gold";
	g_engine = new Engine( config );

	g_app = this;
	m_game = new Game();
	m_game->Startup();

	g_UICamera = new Camera();
	g_UICamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );
	SubscribeEventCallbackFunction( "Quit", App::Event_Quit );
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
	g_engine->BeginFrame();
	Update();
	Render();
	g_engine->EndFrame();
}
//-----------------------------------------------------------------------------------------------

void App::Update()
{

	m_game->Update();
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

void App::SetIsQuitting()
{
	m_game->m_isQuitting = true;
}
//-----------------------------------------------------------------------------------------------

bool App::IsQuitting() const
{
	return m_game->m_isQuitting;
}

//-----------------------------------------------------------------------------------------------
bool App::Event_Quit( [[maybe_unused]] EventArgs& args )
{
	g_theApp->SetIsQuitting();
	return false;
}
