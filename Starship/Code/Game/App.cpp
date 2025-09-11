#include "App.hpp"
#include "PlayerShip.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Engine/Renderer/Camera.hpp"
#include "Game.hpp"


App* g_app = nullptr;
Camera m_gameCamera;

App::App()
{
	g_engine = new Engine;
	g_app = this;
	//m_gameCamera
	m_game = new Game(g_app);
	m_game->Startup();
}

App::~App()
{
	g_engine = nullptr;
	delete g_engine;
	m_game = nullptr;
	delete m_game;
}

void App::RunFrame()
{
	float fakeDeltaSeconds = 1.f / 60.f;
	m_game->Update(fakeDeltaSeconds);
	m_game->Render();
	/*Update*/(fakeDeltaSeconds);
	//Render();
}

void App::Update(float deltaSeconds)
{
	if (m_isSlowMo) // T pressed
		deltaSeconds = 1.f / 600.f; // Run at 1/10th the speed

	if (!m_isPaused || m_pauseAfterNextUpdate) // P not pressed or has a run after O is pressed
	{
		//m_ship1->Update(deltaSeconds);
		//m_ship2->Update(deltaSeconds);
		//m_ship3->Update(deltaSeconds);
		m_pauseAfterNextUpdate = false; // Reset run token for simulation step
	}
}

void App::Render() const
{
	//g_engine->BeginFrame(); // Todo Do BeginCamera instead
	//g_engine->m_render->Startup();
	//g_engine->m_render->BeginCamera(*m_gameCamera);
	/*m_ship1->Render();
	m_ship2->Render();
	m_ship3->Render();*/

	// Todo Render asteroids next

	//g_engine->EndFrame(); // Todo Do EndCamera instead
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
	//m_isKeyDownArray[ keyCode ] = true;
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
	//m_isKeyDownArray[ keyCode ] = false;
	if (keyCode == 'T') // T no longer pressed so no longer slowed
	{
		m_isSlowMo = false;
	}
}

bool App::isKeyDown(unsigned char keyCode)
{
	//return m_isKeyDownArray[ keyCode ];
	return true;
}

bool App::isKeyJustPressed(unsigned char keyCode)
{
	//return m_isKeyDownArray[ keyCode ] && !m_wasKeyDownPrevArray[keyCode];
	return true;
}

bool App::wasKeyJustPressed(unsigned char keyCode)
{
	//return m_isKeyDownArray[ keyCode ] && m_wasKeyDownPrevArray[keyCode];
	return true;
}
