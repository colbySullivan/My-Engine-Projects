#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <vector>
#include <string>

enum DevConsoleMode
{
	HIDDEN,
	OPEN_FULL
};

struct DevConsoleConfig
{
	DevConsoleMode	m_consoleMode = HIDDEN;
};

//-------------------------------------------------------------------------------------
class DevConsole
{
public:
	DevConsole( DevConsoleConfig const& config );
	~DevConsole();
	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void Execute( std::string const& consoleCommandText );
	void AddLine( Rgba8 const& color, std::string const& text );
	void Render( AABB2 const& bounds, Renderer& rendererToUse, BitmapFont& font, float fontAspect = 1.f ) const;

	DevConsoleMode GetMode() const;
	void SetMode( DevConsoleMode mode );
	void ToggleMode( DevConsoleMode mode );

	static const Rgba8 ERROR;        // Color constants like Rgba8::RED; assign these explicit RGB colors!
	static const Rgba8 WARNING;
	static const Rgba8 INFO_MAJOR;
	static const Rgba8 INFO_MINOR;

protected:
	void Render_OpenFull( AABB2 const& bounds, Renderer& rendererToUse, BitmapFont& font, float fontAspect = 1.f ) const;
	// or have the DevConsoleConfig keep a font name
protected:
	DevConsoleConfig                m_config;
	DevConsoleMode                  m_mode = DevConsoleMode::HIDDEN; // also OPEN_FULL, and eventually others
	//std::vector<DevConsoleLine>     m_lines;    // #TODO: support a max limited # of lines (e.g. fixed circular buffer)
	int                             m_frameNumber = 0;
};