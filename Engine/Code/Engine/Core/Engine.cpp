#include "Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <Engine/Input/InputSystem.hpp>

Engine* g_engine = nullptr;
Renderer* m_render;
InputSystem* m_input;

Engine::Engine()
{
	m_render = new Renderer();
	m_render->Startup();
	m_input = new InputSystem();
	m_input->Startup();
}

Engine::~Engine()
{
	m_render->Shutdown();
	delete m_render;
	delete m_input;
}

void Engine::BeginFrame()
{
	m_input->BeginFrame();
	m_render->BeginFrame();
}

void Engine::EndFrame()
{
	m_render->EndFrame();
}
