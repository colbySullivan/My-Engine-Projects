#include "Engine/Core/Engine.hpp"

Engine* g_engine = nullptr;

Engine::Engine( EngineConfig const& config )
{
	g_engine = this;

	m_window = new Window( config.m_windowConfig );
	m_render = new Renderer( config.m_renderConfig);
	m_input = new InputSystem( config.m_inputConfig );
	m_audio = new AudioSystem( config.m_audioConfig );
	m_console = new DevConsole( config.m_devConsoleConfig );
	m_eventSystem = new EventSystem( config.m_eventSystemConfig );

	m_window->Startup();
	m_render->Startup();
	m_input->Startup();
	m_audio->Startup();
	m_console->Startup();
	m_eventSystem->Startup();
}

//-----------------------------------------------------------------------------------------------
Engine::~Engine()
{
	m_window->Shutdown();
	m_input->Shutdown();
	m_render->Shutdown();
	m_audio->Shutdown();
	m_console->Shutdown();
	m_eventSystem->Shutdown();
	delete m_window;
	delete m_input;
	delete m_render;
	delete m_audio;
	delete m_console;
	delete m_eventSystem;
	m_window = nullptr;
	m_input = nullptr;
	m_render = nullptr;
	m_audio = nullptr;
	m_console = nullptr;
	m_eventSystem = nullptr;
}

void Engine::BeginFrame()
{
	m_window->BeginFrame();
	m_input->BeginFrame();
	m_render->BeginFrame();
	m_audio->BeginFrame();
	m_console->BeginFrame();
	m_eventSystem->BeginFrame();
}

void Engine::EndFrame()
{
	m_window->EndFrame();
	m_input->EndFrame();
	m_render->EndFrame();
	m_audio->EndFrame();
	m_console->EndFrame();
	m_eventSystem->EndFrame();
}
