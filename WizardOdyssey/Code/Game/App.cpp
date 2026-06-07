#include "App.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/Game.hpp"

App* g_app = nullptr;

App::App()
{
	EngineConfig config;
	config.m_windowConfig.m_clientAspect = 2.0f;
	config.m_windowConfig.m_windowTitle = "Libra";
	g_engine = new Engine( config );

	g_app = this;
	m_game = new Game();
	LoadXmlMap();

	g_UICamera = new Camera();
	g_UICamera->SetOrthoView(Vec2(0.f, 0.f), Vec2(g_gameConfig->GetValue("screenSizeX", 0.f), g_gameConfig->GetValue("screenSizeY", 0.f)));

	SubscribeEventCallbackFunction("Quit", App::Event_Quit);
	//UnsubscribeEventCallbackFunction("Quit", App::Event_Quit);
}
//-----------------------------------------------------------------------------------------------

App::~App()
{
	m_game = nullptr;
	delete m_game;

	delete g_UICamera;
	g_UICamera = nullptr;

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

void App::Update(float deltaSeconds)
{
	m_game->Update(deltaSeconds);
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
	g_engine->m_console->Render(AABB2(g_UICamera->GetOrthoBottomLeft(), g_UICamera->GetOrthoTopRight()), *m_game->g_testFont, 1.f);
	g_engine->m_render->EndCamera(*g_UICamera);
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

bool App::HandleQuit()
{
	m_game->m_isQuitting = true;
	return false;
}

//------------------------------------------------------------------------------
bool App::Event_Quit( [[maybe_unused]] EventArgs& args) 
{
	g_theApp->HandleQuit();
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