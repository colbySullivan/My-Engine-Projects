#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Game/GameCommon.hpp"


//------------------------------------------------------------------------------
class PlayerShip;
class App;
class Game;
enum GameType;

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
	Game* CreateNewGameOfType( GameType type );
	void Render() const;
	void LoadXmlMap();

	void SetIsQuitting();
	bool IsQuitting() const;
	void UpdateCursorMode();


public:
	Game*		m_game = nullptr;

private:
	float           m_lastFrameTime = 0.f;
};