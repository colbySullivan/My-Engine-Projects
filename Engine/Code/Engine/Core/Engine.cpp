#include "Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <Engine/Input/InputSystem.hpp>

Engine* g_engine = nullptr;

Engine::Engine()
{
	m_render = new Renderer();
	m_render->Startup();
	m_input = new InputSystem();
	m_input->Startup();
}

Engine::~Engine()
{
	m_input->Shutdown();
	m_render->Shutdown();
	delete m_input;
	delete m_render;
}

void Engine::BeginFrame()
{
	m_input->BeginFrame();
	m_render->BeginFrame();
}

void Engine::EndFrame()
{
	m_input->EndFrame();
	m_render->EndFrame();
}
