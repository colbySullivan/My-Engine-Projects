#include "App.hpp"
#include "PlayerShip.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Engine/Renderer/Camera.hpp"
#include "Game.hpp"


App* g_app = nullptr;

App::App()
{
	g_engine = new Engine;
	g_app = this;
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
	Render();
	Update(fakeDeltaSeconds);
}

void App::Update(float deltaSeconds)
{
	m_game->Update(deltaSeconds);
	if (m_isSlowMo) // T pressed
		deltaSeconds = 1.f / 600.f; // Run at 1/10th the speed

	//if (!m_isPaused || m_pauseAfterNextUpdate) // P not pressed or has a run after O is pressed
	//{
	//	m_pauseAfterNextUpdate = false; // Reset run token for simulation step
	//}
	
	// Todo for loop for inputs

}

void App::Render() const
{
	g_engine->m_render->BeginCamera(*m_gameCamera);
	Rgba8 backgroundColor = Rgba8(0.39215686274f, 0.19607843137f, 0.f, 1.f);
	g_engine->m_render->ClearScreen(backgroundColor);
	m_game->Render();
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
	//if (keyCode == ' ') // Runs a single unpaused Update (simulation step) and then pauses.
	//{
	//	m_game->SpawnBullet();
	//}
	m_isKeyDownArray[ keyCode ] = true;
	if(m_isKeyDownArray[ keyCode ])
		m_wasKeyDownPrevArray[ keyCode ] = true;
}

void App::OnKeyUp(unsigned char keyCode)
{
	//m_isKeyDownArray[ keyCode ] = false;
	if (keyCode == 'T') // T no longer pressed so no longer slowed
	{
		m_isSlowMo = false;
	}
	m_isKeyDownArray[ keyCode ] = false;
	if (!m_isKeyDownArray[keyCode])
	{
		m_wasKeyDownPrevArray[ keyCode ] = false;
	}
		
}

bool App::isKeyDown(unsigned char keyCode)
{
	return m_isKeyDownArray[ keyCode ];
}

bool App::isKeyJustPressed(unsigned char keyCode)
{
	return m_isKeyDownArray[ keyCode ] && !m_wasKeyDownPrevArray[keyCode];
}

bool App::wasKeyJustPressed(unsigned char keyCode)
{
	return m_isKeyDownArray[ keyCode ] && m_wasKeyDownPrevArray[keyCode];
}
