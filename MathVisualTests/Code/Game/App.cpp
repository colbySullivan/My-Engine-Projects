#include "App.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/Game.hpp"
#include "Game/GameNearestPoint.hpp"


App* g_app = nullptr;

App::App()
{
	EngineConfig config;
	config.m_windowConfig.m_clientAspect = 2.0f;
	config.m_windowConfig.m_windowTitle = "MathVisualTests";
	g_engine = new Engine( config );
	m_game = new GameNearestPoint( this );
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

void App::Update(float deltaSeconds)
{
	m_game->Update(deltaSeconds);
	if (g_engine->m_input->IsKeyDown(KEYCODE_F7))
	{
		delete m_game;
		m_game = nullptr;
		m_game = new GameNearestPoint(this); //TODO: Change to desired gametype
	}
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

