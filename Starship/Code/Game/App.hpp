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
	Game*		m_game = nullptr;

private:
	bool			m_isKeyDownArray[256];
	bool			m_wasKeyDownPrevArray[256];
	float           m_lastFrameTime = 0.f;
};