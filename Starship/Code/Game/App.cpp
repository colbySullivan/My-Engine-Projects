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
	if (m_isSlowMo) // T pressed
		deltaSeconds = 1.f / 600.f; // Run at 1/10th the speed

	if (!m_isPaused || m_pauseAfterNextUpdate) // P not pressed or has a run after O is pressed
	{
		m_pauseAfterNextUpdate = false; // Reset run token for simulation step
	}
	
	if (isKeyJustPressed('Q'))
	{
		m_isQuitting = true;
	}
	if (isKeyJustPressed('T')) // Slows simulation time to 1/10th the normal rate
	{
		m_isSlowMo = !m_isSlowMo;
	}
	if (isKeyJustPressed('P')) // Pauses game
	{
		m_isPaused = !m_isPaused; // Switch pause
	}
	if (isKeyJustPressed('O')) // Runs a single unpaused Update (simulation step) and then pauses.
	{
		m_isPaused = true;
		m_pauseAfterNextUpdate = true; // Consumed to false after one run of update
	}

	if (isKeyJustPressed('I'))
	{
		m_game->SpawnRandomAsteroid();
	}

	if (isKeyJustPressed('112')) // F1
	{
		g_drawDebug = !g_drawDebug;
	}

	if (isKeyJustPressed('119')) // F8
	{
		g_drawDebug = !g_drawDebug;
	}

	if (m_isSlowMo) // T pressed
		deltaSeconds = 1.f / 600.f; // Run at 1/10th the speed

	if (!m_isPaused || m_pauseAfterNextUpdate)
	{
		m_game->Update(deltaSeconds);
	}

	if (!m_isPaused || m_pauseAfterNextUpdate) // P not pressed or has a run after O is pressed
	{
		m_pauseAfterNextUpdate = false; // Reset run token for simulation step
	}

	for (int i = 0; i < 256; ++ i)
	{
		m_wasKeyDownPrevArray[i] = m_isKeyDownArray[i];
	}
		
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
	m_isKeyDownArray[ keyCode ] = true;
}

void App::OnKeyUp(unsigned char keyCode)
{
	m_isKeyDownArray[ keyCode ] = false;
		
}

bool App::isKeyDown(unsigned char keyCode)
{
	return m_isKeyDownArray[ keyCode ];
}

bool App::isKeyJustPressed(unsigned char keyCode)
{
	return !m_isKeyDownArray[ keyCode ] && m_wasKeyDownPrevArray[keyCode];
}

bool App::wasKeyJustPressed(unsigned char keyCode)
{
	return m_isKeyDownArray[ keyCode ] && m_wasKeyDownPrevArray[keyCode];
}
