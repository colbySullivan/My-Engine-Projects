#include "App.hpp";
#include "PlayerShip.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"  

App* g_app = nullptr;

App::App()
{
	g_engine = new Engine;
	//m_game = new Game;
	m_ship1 = new PlayerShip(Vec2(0.f, 30.f), Vec2(12.f, 0.f));
	m_ship2 = new PlayerShip(Vec2(0.f, 50.f), Vec2(20.f, 0.f));
	m_ship3 = new PlayerShip(Vec2(0.f, 70.f), Vec2(15.f, 0.f));
}

App::~App()
{
	g_engine = nullptr; 
	delete g_engine;
	/*m_game = nullptr;
	delete m_game;*/
	delete m_ship1;   m_ship1 = nullptr;
	delete m_ship2;   m_ship2 = nullptr;
	delete m_ship3;   m_ship3 = nullptr;
}

void App::RunFrame()
{
	float fakeDeltaSeconds = 1.f / 60.f;
	Update(fakeDeltaSeconds);
	Render();
}

void App::Update(float deltaSeconds)
{
	if (m_isSlowMo) // T pressed
		deltaSeconds = 1.f / 600.f; // Run at 1/10th the speed

	if (!m_isPaused || m_pauseAfterNextUpdate) // P not pressed or has a run after O is pressed
	{
		m_ship1->Update(deltaSeconds);
		m_ship2->Update(deltaSeconds);
		m_ship3->Update(deltaSeconds);
		m_pauseAfterNextUpdate = false; // Reset run token for simulation step
	}
}

void App::Render() const
{
	g_engine->BeginFrame();
	//g_engine->m_render->Startup();
	//g_engine->m_render->BeginCamera(m_gameCamera);
	m_ship1->Render();
	m_ship2->Render();
	m_ship3->Render();
	g_engine->EndFrame();
}

void App::SetIsQuitting()
{
	m_isQuitting = true;
}

bool App::IsQuitting() const
{
	return m_isQuitting;
}

void App::OnKeyDown(unsigned char keyCode)
{
	// #SD1ToDo: Tell the App (or InputSystem later) about this key-pressed event...
	if (keyCode == 'Q') // #SD1ToDo: move this "check for ESC pressed" code to App
	{
		m_isQuitting = true;
	}
	if (keyCode == 'T') // Slows simulation time to 1/10th the normal rate
	{
		m_isSlowMo = true;
	}
	if (keyCode == 'P') // Pauses game
	{
		m_isPaused = !m_isPaused; // Switch pause
	}
	if (keyCode == 'O') // Runs a single unpaused Update (simulation step) and then pauses.
	{
		m_isPaused = true;
		m_pauseAfterNextUpdate = true; // Consumed to false after one run of update
	}
}

void App::OnKeyUp(unsigned char keyCode)
{
	//Todo this is broken
	if (keyCode == 'T') // T no longer pressed so no longer slowed
	{
		m_isSlowMo = false;
	}
}
