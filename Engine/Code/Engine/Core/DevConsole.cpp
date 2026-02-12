#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Engine.hpp" 
#include "Engine/Core/VertexUtils.hpp"

const Rgba8 DevConsole::ERROR_COLOR = Rgba8(255, 50, 50);		
const Rgba8 DevConsole::WARNING_COLOR = Rgba8(255, 200, 0);		
const Rgba8 DevConsole::COMMAND_COLOR = Rgba8(0, 255, 0);		
const Rgba8 DevConsole::INFO_MAJOR_COLOR = Rgba8(100, 200, 255);
const Rgba8 DevConsole::INFO_MAJOR_COLOR_TINT = Rgba8(100, 200, 255, 150);
const Rgba8 DevConsole::INFO_MINOR_COLOR = Rgba8(180, 180, 180);

DevConsole* g_DevConsole = nullptr;


//-----------------------------------------------------------------------------------------------
DevConsole::DevConsole( DevConsoleConfig const& config )
	: m_config( config )
{
	g_DevConsole = this;
}

//-----------------------------------------------------------------------------------------------
DevConsole::~DevConsole()
{

}

//-----------------------------------------------------------------------------------------------
void DevConsole::Startup()
{
	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "KeyPressed", DevConsole::Event_KeyPressed );
	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "CharPressed", DevConsole::Event_CharInput );
	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "clear", DevConsole::Command_Clear );
	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "help", DevConsole::Command_Help );
	m_lines.clear();
	m_mode = m_config.m_consoleMode;
}

//-----------------------------------------------------------------------------------------------
void DevConsole::Shutdown()
{

}

//-----------------------------------------------------------------------------------------------
void DevConsole::BeginFrame()
{
	m_frameNumber += 1;
}

//-----------------------------------------------------------------------------------------------
void DevConsole::EndFrame()
{

}

//-----------------------------------------------------------------------------------------------
void DevConsole::Execute( [[maybe_unused]] std::string const& consoleCommandText )
{
	std::vector<std::string> registeredCommands = g_engine->m_eventSystem->GetAllRegisteredCommands();
	auto it = std::find( registeredCommands.begin(), registeredCommands.end(), consoleCommandText );

	if ( it != registeredCommands.end() )
	{
		AddLine( COMMAND_COLOR, consoleCommandText, 20.f, 0.f );
		FireEvent( consoleCommandText );
	}
	else
	{
		AddLine( COMMAND_COLOR, consoleCommandText, 20.f, 0.f );
		AddLine( ERROR_COLOR, "Unknown command: " + consoleCommandText, 20.f, 0.f);
	}
}

//------------------------------------------------------------------------------
void DevConsole::AddLine(Rgba8 const& color, std::string const& text, float cellheight, float timestamp)
{
	DevConsoleLine newLine;
	newLine.m_color = color;
	newLine.m_text = text;
	newLine.m_cellHeight = cellheight;
	newLine.m_frameNumber = m_frameNumber;
	newLine.m_timestamp = timestamp;
	m_lines.push_back(newLine);
}

//-----------------------------------------------------------------------------------------------
void DevConsole::Render(AABB2 const& bounds, BitmapFont& font, float fontAspect /*= 1.f */) const
{
	switch (m_mode)
	{
	case HIDDEN:
		g_engine->m_render->BindTexture( nullptr );
		return;

	case OPEN_FULL:
		Render_OpenFull(bounds, font, fontAspect);
		break;
	}
}

//-----------------------------------------------------------------------------------------------
DevConsoleMode DevConsole::GetMode() const
{
	return m_mode;
}

//-----------------------------------------------------------------------------------------------
void DevConsole::SetMode( DevConsoleMode mode )
{
	m_mode = mode;
}

//-----------------------------------------------------------------------------------------------
void DevConsole::ToggleMode( [[maybe_unused]] DevConsoleMode mode )
{
	int modeNumber = static_cast<int>(m_mode);
	m_mode = static_cast<DevConsoleMode>((modeNumber + 1) % NUM_CONSOLE_MODES);
	m_lines.clear();
	m_inputText.clear();
	AddLine( WARNING_COLOR, "Type help for list of commands", 20.f, 0.f );
}

//-----------------------------------------------------------------------------------------------
void DevConsole::Render_OpenFull(AABB2 const& bounds, BitmapFont& font, float fontAspect) const
{
	g_engine->m_render->BindTexture( nullptr );
	std::vector<Vertex> boxVerts;
	AddVertsForAABB2D(boxVerts, bounds, Rgba8(0, 0, 0, 200));
	g_engine->m_render->DrawVertexArray(boxVerts);

	g_engine->m_render->BindTexture( &font.GetTexture() );
	std::vector<Vertex> textVerts;

	float cellHeight = 20.f;
	if ( m_lines.size() > 0 )
	{
		cellHeight = m_lines[0].m_cellHeight;
	}

	AABB2 inputBounds;
	inputBounds.m_mins = bounds.m_mins;
	inputBounds.m_maxs = Vec2( bounds.m_maxs.x, bounds.m_mins.y + cellHeight );
	font.AddVertsForTextInBox2D( textVerts, m_inputText, inputBounds, cellHeight, INFO_MINOR_COLOR, fontAspect, Vec2( 0.f, 0.f ), TextBoxMode::SHRINK_TO_FIT );

	float currentY = bounds.m_mins.y + cellHeight;
	for ( int lineIndex = ( int )m_lines.size() - 1; lineIndex >= 0; lineIndex-- )
	{
		DevConsoleLine const& line = m_lines[lineIndex];
		AABB2 lineBounds;
		lineBounds.m_mins = Vec2( bounds.m_mins.x, currentY );
		lineBounds.m_maxs = Vec2( bounds.m_maxs.x, currentY + line.m_cellHeight );
		font.AddVertsForTextInBox2D( textVerts, line.m_text, lineBounds, line.m_cellHeight, line.m_color, fontAspect, Vec2( 0.f, 0.f ), TextBoxMode::SHRINK_TO_FIT );
		currentY += line.m_cellHeight;

		if ( currentY > bounds.m_maxs.y )
		{
			break;
		}
	}
	g_engine->m_render->DrawVertexArray(textVerts);
}

//-----------------------------------------------------------------------------------------------
bool DevConsole::Event_KeyPressed( EventArgs& args )
{
	if ( !g_engine->m_input || g_DevConsole->m_mode == HIDDEN)
	{
		return false;
	}
	unsigned char keyCode = ( unsigned char )args.GetValue( "KeyCode", -1 );
	g_engine->m_input->HandleKeyPressed( keyCode );
	return true;
}

//-----------------------------------------------------------------------------------------------
bool DevConsole::Event_CharInput( EventArgs& args )
{
	if ( !g_engine->m_input || g_DevConsole->m_mode == HIDDEN )
	{
		return false;
	}
	unsigned char charCode = ( unsigned char )args.GetValue( "KeyCode", -1 );
	if ( charCode == KEYCODE_ENTER )
	{
		if ( !g_DevConsole->m_inputText.empty() )
		{
			g_DevConsole->Execute( g_DevConsole->m_inputText );
			g_DevConsole->m_inputText.clear();
		}
		return true;
	}

	if ( charCode == KEYCODE_BACKSPACE )
	{
		if ( !g_DevConsole->m_inputText.empty() )
		{
			g_DevConsole->m_inputText.pop_back();
		}
		return true;
	}

	if ( charCode == KEYCODE_ESC )
	{
		g_DevConsole->m_inputText.clear();
		return true;
	}

	if ( charCode >= 32 && charCode <= 126 )
	{
		g_DevConsole->m_inputText += static_cast< char >( charCode );
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------------------------
bool DevConsole::Command_Clear( EventArgs& args )
{
	if ( !g_engine->m_input || g_DevConsole->m_mode == HIDDEN )
	{
		return false;
	}
	g_DevConsole->m_lines.clear();
	return true;
}

//-----------------------------------------------------------------------------------------------
bool DevConsole::Command_Help( EventArgs& args )
{
	if ( !g_engine->m_input || g_DevConsole->m_mode == HIDDEN )
	{
		return false;
	}
	

	std::vector<std::string> registeredCommands = g_engine->m_eventSystem->GetAllRegisteredCommands();
	g_DevConsole->AddLine( INFO_MAJOR_COLOR, "Registered Commands", 20.f, 0.f);
	for ( const std::string& command : registeredCommands )
	{
		g_DevConsole->AddLine( INFO_MAJOR_COLOR_TINT, command, 20.f, 0.f );
	}
	return true;
}