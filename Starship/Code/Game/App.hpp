#pragma once
#include <Engine/Renderer/Camera.hpp>
#include "Game.hpp"


//------------------------------------------------------------------------------
class PlayerShip;
class App;
class Game;

//------------------------------------------------------------------------------
extern App* g_theApp;

//------------------------------------------------------------------------------
class App
{
public:
	App();
	~App();

	void RunFrame();
	void Update(float deltaSeconds);
	void Render() const;

	void SetIsQuitting();
	bool IsQuitting() const;


public:
	//Camera*     m_gameCamera = nullptr;
	Game*		m_game = nullptr;

private:
	//bool            m_isQuitting = false;
	//bool            m_isPaused = false;
	//bool            m_isSlowMo = false;
	//bool            m_pauseAfterNextUpdate = false;

	bool			m_isKeyDownArray[256];
	bool			m_wasKeyDownPrevArray[256];
};