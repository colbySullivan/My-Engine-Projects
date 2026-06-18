#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/ImGuiSystem.hpp"

//------------------------------------------------------------------------------
class Engine;
class Window;
class Renderer;
class InputSystem;
class AudioSystem;
class DevConsole;
class EventSystem;
class Clock;
class ImGuiSystem;

//------------------------------------------------------------------------------
extern Engine* g_engine; // Advertisement that this global exists, so external people can use it

//-----------------------------------------------------------------------------------------------
struct EngineConfig
{
    AudioConfig       m_audioConfig;
    InputConfig       m_inputConfig;
    RenderConfig      m_renderConfig;
    WindowConfig      m_windowConfig;
	DevConsoleConfig  m_devConsoleConfig;
	EventSystemConfig m_eventSystemConfig;
	ImGuiConfig       m_imguiConfig;
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
    Window*         m_window        = nullptr;
    Renderer*       m_render        = nullptr;
	InputSystem*    m_input         = nullptr;
    AudioSystem*    m_audio         = nullptr;
    DevConsole*     m_console       = nullptr;
    EventSystem*    m_eventSystem   = nullptr;
    Clock*          m_systemClock   = nullptr;
    ImGuiSystem*    m_imgui         = nullptr;
};