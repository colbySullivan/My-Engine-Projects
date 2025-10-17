#pragma once

//------------------------------------------------------------------------------
class Engine;   // Forward declaration; simply states that "an Engine class exists"
class Window;   // Forward declaration; simply states that "a Renderer class exists"
class Renderer; // Forward declaration; simply states that "a Renderer class exists"
class InputSystem; // Forward declaration; simply states that "an InputSystem class exists" 
class AudioSystem;

//------------------------------------------------------------------------------
extern Engine* g_engine; // Advertisement that this global exists, so external people can use it

//------------------------------------------------------------------------------
class Engine
{
public:
    Engine();
    ~Engine();
    void BeginFrame();
    void EndFrame();

public:
    Window*         m_window    = nullptr;
    Renderer*       m_render    = nullptr;
	InputSystem*    m_input     = nullptr; 
    AudioSystem*    m_audio     = nullptr;
};