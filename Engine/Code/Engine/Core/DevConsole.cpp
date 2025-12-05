#include "Engine/Core/DevConsole.hpp"

const Rgba8 DevConsole::ERROR;
const Rgba8 DevConsole::WARNING;
const Rgba8 DevConsole::INFO_MAJOR;
const Rgba8 DevConsole::INFO_MINOR;

//-----------------------------------------------------------------------------------------------
DevConsole::DevConsole( DevConsoleConfig const& config )
	: m_config( config )
{

}

//-----------------------------------------------------------------------------------------------
DevConsole::~DevConsole()
{

}

//-----------------------------------------------------------------------------------------------
void DevConsole::Startup()
{

}

//-----------------------------------------------------------------------------------------------
void DevConsole::Shutdown()
{

}

//-----------------------------------------------------------------------------------------------
void DevConsole::BeginFrame()
{

}

//-----------------------------------------------------------------------------------------------
void DevConsole::EndFrame()
{

}

//-----------------------------------------------------------------------------------------------
void DevConsole::Execute( std::string const& consoleCommandText )
{

}

//-----------------------------------------------------------------------------------------------
void DevConsole::AddLine( Rgba8 const& color, std::string const& text )
{

}

//-----------------------------------------------------------------------------------------------
void DevConsole::Render( AABB2 const& bounds, Renderer& rendererToUse, BitmapFont& font, float fontAspect /*= 1.f */ ) const
{

}

//-----------------------------------------------------------------------------------------------
DevConsoleMode DevConsole::GetMode() const
{
	return m_config.m_consoleMode;
}

//-----------------------------------------------------------------------------------------------
void DevConsole::SetMode( DevConsoleMode mode )
{

}

//-----------------------------------------------------------------------------------------------
void DevConsole::ToggleMode( DevConsoleMode mode )
{

}

//-----------------------------------------------------------------------------------------------
void DevConsole::Render_OpenFull( AABB2 const& bounds, Renderer& rendererToUse, BitmapFont& font, float fontAspect /*= 1.f */ ) const
{

}