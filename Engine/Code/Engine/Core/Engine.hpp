#pragma once

//------------------------------------------------------------------------------
class Engine;   // Forward declaration; simply states that "an Engine class exists"
class Renderer; // Forward declaration; simply states that "a Renderer class exists"
class InputSystem; // Forward declaration; simply states that "an InputSystem class exists" 

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
    Renderer*   m_render    = nullptr; // Legal because of line 5
	InputSystem* m_input = nullptr; // Legal because of line 6  
};