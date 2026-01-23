#include "Engine/Core/Engine.hpp"

Engine* g_engine = nullptr;

Engine::Engine( EngineConfig const& config )
{
	g_engine = this;

	if( config.m_windowConfig.m_isEnabled )			m_window = new Window( config.m_windowConfig );
	if( config.m_renderConfig.m_isEnabled )			m_render = new Renderer( config.m_renderConfig);
	if( config.m_inputConfig.m_isEnabled )			m_input = new InputSystem( config.m_inputConfig );
	if( config.m_audioConfig.m_isEnabled )			m_audio = new AudioSystem( config.m_audioConfig );
	if( config.m_devConsoleConfig.m_isEnabled )		m_console = new DevConsole( config.m_devConsoleConfig );
	if( config.m_eventSystemConfig.m_isEnabled )	m_eventSystem = new EventSystem( config.m_eventSystemConfig );

	if( m_window )			m_window->Startup();
	if( m_render )			m_render->Startup();
	if( m_input )			m_input->Startup();
	if( m_audio )			m_audio->Startup();
	if( m_console )			m_console->Startup();
	if( m_eventSystem )		m_eventSystem->Startup();
}

//-----------------------------------------------------------------------------------------------
Engine::~Engine()
{
	if( m_window )			m_window->Shutdown();
	if( m_input )			m_input->Shutdown();
	if( m_render )			m_render->Shutdown();
	if( m_audio )			m_audio->Shutdown();
	if( m_console )			m_console->Shutdown();
	if( m_eventSystem )		m_eventSystem->Shutdown();
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
	if( m_window )			m_window->BeginFrame();
	if( m_input )			m_input->BeginFrame();
	if( m_render )			m_render->BeginFrame();
	if( m_audio )			m_audio->BeginFrame();
	if( m_console )			m_console->BeginFrame();
	if( m_eventSystem )		m_eventSystem->BeginFrame();
}

void Engine::EndFrame()
{
	if( m_render )			m_render->EndFrame();
	if( m_window )			m_window->EndFrame();
	if( m_input )			m_input->EndFrame();
	if( m_audio )			m_audio->EndFrame();
	if( m_console )			m_console->EndFrame();
	if( m_eventSystem )		m_eventSystem->EndFrame();
}
