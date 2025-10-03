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
	g_app = this;
	m_game = new Game();
	m_game->Startup();
}
//-----------------------------------------------------------------------------------------------

App::~App()
{
	m_game = nullptr;
	delete m_game;
}
//-----------------------------------------------------------------------------------------------

void App::RunFrame()
{
	float timeNow = (float)GetCurrentTimeSeconds();
	float deltaSeconds = timeNow - m_lastFrameTime;
	m_lastFrameTime = timeNow;

	g_engine->BeginFrame();
	Render();
	Update(deltaSeconds);
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

