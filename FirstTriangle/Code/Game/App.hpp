#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/EventSystem.hpp"


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
	static bool Event_Quit( EventArgs& args );

	void SetIsQuitting();
	bool IsQuitting() const;

private:
	bool			m_isQuitting = false;
	float           m_lastFrameTime = 0.f;
};