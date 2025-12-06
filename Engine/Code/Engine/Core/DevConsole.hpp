#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <vector>
#include <string>

enum DevConsoleMode
{
	HIDDEN,
	OPEN_FULL,
	NUM_CONSOLE_MODES
};

struct DevConsoleConfig
{
	DevConsoleMode	m_consoleMode = HIDDEN;
};

struct DevConsoleLine
{
	Rgba8       m_color;
	std::string m_text;
	float		m_cellHeight = 20.f;
	float 		m_timestamp = 0.f;
	int			m_frameNumber = 0;
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
	void AddLine( Rgba8 const& color, std::string const& text, float cellheight = 20.f, float timestamp = 0.f );
	void Render( AABB2 const& bounds, BitmapFont& font, float fontAspect = 1.f ) const;

	DevConsoleMode GetMode() const;
	void SetMode( DevConsoleMode mode );
	void ToggleMode( DevConsoleMode mode );

	static const Rgba8 ERROR_COLOR;        // Color constants like Rgba8::RED; assign these explicit RGB colors!
	static const Rgba8 WARNING_COLOR;
	static const Rgba8 INFO_MAJOR_COLOR;
	static const Rgba8 INFO_MINOR_COLOR;

protected:
	void Render_OpenFull( AABB2 const& bounds, BitmapFont& font, float fontAspect = 1.f ) const;
	// or have the DevConsoleConfig keep a font name
protected:
	DevConsoleConfig                m_config;
	DevConsoleMode                  m_mode = DevConsoleMode::HIDDEN; // also OPEN_FULL, and eventually others
	std::vector<DevConsoleLine>     m_lines;    // #TODO: support a max limited # of lines (e.g. fixed circular buffer)
	int                             m_frameNumber = 0;
};