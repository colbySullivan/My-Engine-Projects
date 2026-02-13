#pragma once
#include <Engine/Renderer/Camera.hpp>
#include "Engine/Core/EventSystem.hpp"
#include "Game.hpp"


//------------------------------------------------------------------------------
class PlayerShip;
class App;
class Game;
class Clock;

//------------------------------------------------------------------------------
extern App* g_theApp;

//------------------------------------------------------------------------------
class App
{
public:
	App();
	~App();

	void RunFrame();
	void Update();
	void Render() const;
	static bool Event_Quit( EventArgs& args );

	void SetIsQuitting();
	bool IsQuitting() const;


public:
	Game*		m_game = nullptr;

private:
	float           m_lastFrameTime = 0.f;
	Camera*			g_UICamera = nullptr;
};