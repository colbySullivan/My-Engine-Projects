#include "Engine/Core/Engine.hpp"
#include "Engine/Window/Window.hpp"

Engine* g_engine = nullptr;

Engine::Engine( EngineConfig const& config )
{
	g_engine = this;

	m_window = new Window( config.m_windowConfig );
	m_render = new Renderer( config.m_renderConfig);
	m_input = new InputSystem( config.m_inputConfig );
	m_audio = new AudioSystem( config.m_audioConfig );

	m_window->Startup();
	m_render->Startup();
	m_input->Startup();
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
	m_audio->BeginFrame();
}

void Engine::EndFrame()
{
	m_window->EndFrame();
	m_input->EndFrame();
	m_render->EndFrame();
	m_audio->EndFrame();
}
