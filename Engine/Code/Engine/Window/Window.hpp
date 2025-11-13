#pragma once
#include <string>
#include "Engine/Math/Vec2.hpp"

//------------------------------------------------------------------------------
struct Rgba8;
struct Vertex;
class Camera;
//-----------------------------------------------------------------------------------------------
struct WindowConfig
{
	bool			m_isEnabled = true;
	float			m_clientAspect = (16.0f / 9.0f );
	std::string		m_windowTitle = "Untitled Colby Project";
};

//------------------------------------------------------------------------------
class Window
{
public:
	Window(  WindowConfig const& config );
	~Window();
	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();
	Vec2 GetNormalizedMouseUV() const;

	WindowConfig	m_config;
	void*			m_displayDeviceContext = 0;
	void*			m_windowHandle = 0;
private:
	void CreateOSWindow();
	void RunMessagePump();
};