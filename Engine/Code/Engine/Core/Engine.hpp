#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Core/DevConsole.hpp"

//------------------------------------------------------------------------------
class Engine;   // Forward declaration; simply states that "an Engine class exists"
class Window;   // Forward declaration; simply states that "a Renderer class exists"
class Renderer; // Forward declaration; simply states that "a Renderer class exists"
class InputSystem; // Forward declaration; simply states that "an InputSystem class exists" 
class AudioSystem;
class DevConsole;

//------------------------------------------------------------------------------
extern Engine* g_engine; // Advertisement that this global exists, so external people can use it

//-----------------------------------------------------------------------------------------------
struct EngineConfig
{
    AudioConfig m_audioConfig;
    InputConfig m_inputConfig;
    RenderConfig m_renderConfig;
    WindowConfig m_windowConfig;
	DevConsoleConfig m_devConsoleConfig;
};

//------------------------------------------------------------------------------
class Engine
{
public:
    Engine( EngineConfig const& config );
    ~Engine();
    void BeginFrame();
    void EndFrame();

public:
    Window*         m_window    = nullptr;
    Renderer*       m_render    = nullptr;
	InputSystem*    m_input     = nullptr; 
    AudioSystem*    m_audio     = nullptr;
    DevConsole*     m_console   = nullptr;
};