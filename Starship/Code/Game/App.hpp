#pragma once
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Camera.hpp"

//------------------------------------------------------------------------------
class PlayerShip;
class App;

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
	void OnKeyDown(unsigned char keyCode);
	void OnKeyUp(unsigned char keyCode);

public:
	PlayerShip* m_ship1 = nullptr;
	PlayerShip* m_ship2 = nullptr;
	PlayerShip* m_ship3 = nullptr;
	Camera          m_gameCamera;

	bool            m_isQuitting = false;
	bool            m_isPaused = false;
	bool            m_isSlowMo = false;
	bool            m_pauseAfterNextUpdate = false;
};