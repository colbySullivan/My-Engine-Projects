#pragma once
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
	//void Shutdown();
	void RunFrame();
	void Update(float deltaSeconds);
	void Render() const;

	void SetIsQuitting();
	bool IsQuitting() const;

private:
	bool			m_isQuitting = false;
	float           m_lastFrameTime = 0.f;
};