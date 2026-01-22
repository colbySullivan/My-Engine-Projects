#pragma once
#include "Engine/Renderer/Camera.hpp"


//------------------------------------------------------------------------------
class PlayerShip;
class App;
class Window;

//------------------------------------------------------------------------------
extern App* g_theApp;
extern Window* g_theWindow;

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

private:
	bool			m_isQuitting = false;
	float           m_lastFrameTime = 0.f;
};