#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Game.hpp"


//------------------------------------------------------------------------------
class Player;
class App;
class Game;
class NamedStrings;

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
	bool HandleQuit();
	static bool Event_Quit(EventArgs& args);
	void LoadXmlMap();


public:
	Game*		m_game = nullptr;

private:
	float           m_lastFrameTime = 0.f;
	Camera*			g_UICamera = nullptr;
};