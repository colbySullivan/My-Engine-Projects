#include "App.hpp"
#include "PlayerShip.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Engine/Renderer/Camera.hpp"
#include "Game.hpp"


App* g_app = nullptr;

App::App()
{
	g_app = this;
	m_game = new Game(g_app);
	m_game->Startup();
}

App::~App()
{
	m_game = nullptr;
	delete m_game;
}

void App::RunFrame()
{
	float fakeDeltaSeconds = 1.f / 60.f;
	//g_engine->BeginFrame();
	Render();
	Update(fakeDeltaSeconds);
}

void App::Update(float deltaSeconds)
{
	m_game->Update(deltaSeconds);
}

void App::Render() const
{
	m_game->Render();
}

void App::SetIsQuitting()
{
	m_game->m_isQuitting = true;
}

bool App::IsQuitting() const
{
	return m_game->m_isQuitting;
}

