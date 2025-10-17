#pragma once

//------------------------------------------------------------------------------
struct Rgba8;
struct Vertex;
class Camera;

//------------------------------------------------------------------------------
class Window
{
public:
	Window();
	~Window();
	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();
};