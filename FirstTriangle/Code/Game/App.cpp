#include "App.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"


App* g_app = nullptr;

App::App()
{
	EngineConfig config;
	config.m_windowConfig.m_clientAspect = 2.0f;
	config.m_windowConfig.m_windowTitle = "FirstTriangle";
	g_engine = new Engine( config );
	g_app = this;
}
//-----------------------------------------------------------------------------------------------

App::~App()
{
}
//-----------------------------------------------------------------------------------------------

void App::RunFrame()
{
	float timeNow = (float)GetCurrentTimeSeconds();
	float deltaSeconds = timeNow - m_lastFrameTime;
	m_lastFrameTime = timeNow;

	g_engine->BeginFrame();
	Update(deltaSeconds);
	Render();
}
//-----------------------------------------------------------------------------------------------

void App::Update([[maybe_unused]] float deltaSeconds)
{
	if ( g_engine->m_input->IsKeyDown( KEYCODE_ESC ) )
	{
		SetIsQuitting();
	}
}
//-----------------------------------------------------------------------------------------------

void App::Render() const
{
}
//-----------------------------------------------------------------------------------------------

void App::SetIsQuitting()
{
	m_isQuitting = true;
}
//-----------------------------------------------------------------------------------------------

bool App::IsQuitting() const
{
	return m_isQuitting;
}

