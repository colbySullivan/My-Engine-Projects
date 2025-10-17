#include "Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Window/Window.hpp"

Engine* g_engine = nullptr;

Engine::Engine()
{
	m_window = new Window();
	m_window->Startup();
	m_render = new Renderer();
	m_render->Startup();
	m_input = new InputSystem();
	m_input->Startup();
	m_audio = new AudioSystem();
	m_audio->Startup();
}

//-----------------------------------------------------------------------------------------------
Engine::~Engine()
{
	m_window->Shutdown();
	m_input->Shutdown();
	m_render->Shutdown();
	m_audio->Shutdown();
	delete m_window;
	delete m_input;
	delete m_render;
	delete m_audio;
	m_window = nullptr;
	m_input = nullptr;
	m_render = nullptr;
	m_audio = nullptr;
}

void Engine::BeginFrame()
{
	m_window->BeginFrame();
	m_input->BeginFrame();
	m_render->BeginFrame();
}

void Engine::EndFrame()
{
	m_window->EndFrame();
	m_input->EndFrame();
	m_render->EndFrame();
}
