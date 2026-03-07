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
	m_insertionPointBlinkTimer = new Timer(0.5f);
	m_insertionPointBlinkTimer->Start();
	AddLine( WARNING_COLOR, "Type help for list of commands", 20.f, 0.f );
}

//-----------------------------------------------------------------------------------------------
void DevConsole::Shutdown()
{

}

//-----------------------------------------------------------------------------------------------
void DevConsole::BeginFrame()
{
	m_frameNumber += 1;
	if ( m_insertionPointBlinkTimer->DecrementPeriodIfElapsed() )
	{
		m_insertionPointBlinkTimer->Start();
		m_insertionPointVisible = !m_insertionPointVisible;
	}
}

//-----------------------------------------------------------------------------------------------
void DevConsole::EndFrame()
{

}

//-----------------------------------------------------------------------------------------------
void DevConsole::Execute( [[maybe_unused]] std::string const& consoleCommandText )
{
	std::vector<std::string> spaceSplit = SplitStringOnDelimiter( consoleCommandText, ' ' );
	std::vector<std::string> registeredCommands = g_engine->m_eventSystem->GetAllRegisteredCommands();
	std::vector<std::string> parsedCommands;
	for ( int i = 0; i < spaceSplit.size(); ++i )
	{
		std::vector<std::string> equalSplit = SplitStringOnDelimiter( spaceSplit[i], '=' );
		std::string command = equalSplit[0];
		auto it = std::find( registeredCommands.begin(), registeredCommands.end(), command );
		EventArgs args = {};
		if ( it != registeredCommands.end() )
		{
			for ( int equalSplitIndex = 1; equalSplitIndex < equalSplit.size(); ++equalSplitIndex )
			{
				args.SetValue( Stringf( "%d", ( unsigned char )equalSplitIndex ), equalSplit[equalSplitIndex]);
			}

			AddLine( COMMAND_COLOR, command, 20.f, 0.f );
			FireEvent( command, args );
		}
		else
		{
			AddLine( COMMAND_COLOR, command, 20.f, 0.f );
			AddLine( ERROR_COLOR, "Unknown command: " + command, 20.f, 0.f);
		}

		if ( m_commandHistory.empty() || m_commandHistory.back() != consoleCommandText )
		{
			m_commandHistory.push_back( consoleCommandText );

			if ( m_commandHistory.size() > m_maxCommandHistory )
			{
				m_commandHistory.erase( m_commandHistory.begin() );
			}
		}
	}
	g_DevConsole->m_historyIndex = -1;
	m_insertionPointPosition = 0;
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
	g_DevConsole->m_historyIndex = -1;
}

//-----------------------------------------------------------------------------------------------
void DevConsole::Render_OpenFull(AABB2 const& bounds, BitmapFont& font, float fontAspect) const
{
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
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

	AABB2 inputBoundsWithCursor;
	inputBoundsWithCursor.m_mins = Vec2(bounds.m_mins.x - cellHeight * 0.4f, bounds.m_mins.y);
	inputBoundsWithCursor.m_maxs = Vec2( bounds.m_maxs.x, bounds.m_mins.y + cellHeight );

	std::string inputWithCursor( m_inputText.size(), ' ' );
	inputWithCursor.insert( m_insertionPointPosition, "|" );
	if ( m_insertionPointVisible )
	{
		font.AddVertsForTextInBox2D( textVerts, inputWithCursor, inputBoundsWithCursor, cellHeight, Rgba8(255,255,255,255), fontAspect, Vec2(0.f, 0.f), TextBoxMode::SHRINK_TO_FIT);
	}

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
	unsigned char keyCode = ( unsigned char )args.GetValue( "KeyCode", -1 );

	if ( !g_engine->m_input || g_DevConsole->m_mode == HIDDEN)
	{
		if ( keyCode == KEYCODE_TILDA )
		{
			//g_DevConsole->m_insertionPointPosition = 0;
			g_DevConsole->ToggleMode( HIDDEN );
			return true;
		}
		return false;
	}

	g_DevConsole->m_insertionPointBlinkTimer->Start();
	g_DevConsole->m_insertionPointVisible = true;

	if ( keyCode == KEYCODE_ESC )
	{
		if (!g_DevConsole->m_inputText.empty() )
		{
			g_DevConsole->m_insertionPointPosition = 0;
			g_DevConsole->m_inputText.clear();
		}
		else
		{
			g_DevConsole->m_insertionPointPosition = 0;
			g_DevConsole->m_historyIndex = -1;
			g_DevConsole->ToggleMode( HIDDEN );
		}
		return true;
	}

	if ( keyCode == KEYCODE_TILDA )
	{
		g_DevConsole->m_historyIndex = -1;
		g_DevConsole->ToggleMode( HIDDEN );
		return true;
	}

	if ( keyCode == KEYCODE_UPARROW )
	{
		if ( !g_DevConsole->m_commandHistory.empty() )
		{
			if ( g_DevConsole->m_historyIndex == -1 )
			{
				g_DevConsole->m_historyIndex = (int)g_DevConsole->m_commandHistory.size() - 1;
			}
			else if ( g_DevConsole->m_historyIndex > 0 )
			{
				g_DevConsole->m_historyIndex--;
			}

			if ( g_DevConsole->m_historyIndex >= 0 )
			{
				g_DevConsole->m_inputText = g_DevConsole->m_commandHistory[g_DevConsole->m_historyIndex];
				g_DevConsole->m_insertionPointPosition = static_cast<int>(g_DevConsole->m_inputText.size());
			}
		}
		return true;
	}

	if ( keyCode == KEYCODE_DOWNARROW )
	{
		if ( !g_DevConsole->m_commandHistory.empty() && g_DevConsole->m_historyIndex != -1 )
		{
			g_DevConsole->m_historyIndex++;

			if ( g_DevConsole->m_historyIndex >= g_DevConsole->m_commandHistory.size() )
			{
				g_DevConsole->m_insertionPointPosition = 0;
				g_DevConsole->m_historyIndex = -1;
				g_DevConsole->m_inputText.clear();
			}
			else
			{
				g_DevConsole->m_inputText = g_DevConsole->m_commandHistory[g_DevConsole->m_historyIndex];
				g_DevConsole->m_insertionPointPosition = static_cast< int >( g_DevConsole->m_inputText.size() );

			}
		}
		return true;
	}

	if ( keyCode == KEYCODE_RIGHTARROW )
	{
		if ( g_DevConsole->m_insertionPointPosition < g_DevConsole->m_inputText.size() )
		{
			g_DevConsole->m_insertionPointPosition++;
		}
	}

	if ( keyCode == KEYCODE_LEFTARROW )
	{
		if ( g_DevConsole->m_insertionPointPosition > 0 )
		{
			g_DevConsole->m_insertionPointPosition--;
		}
	}
	
	if ( keyCode == KEYCODE_ENTER )
	{
		if ( !g_DevConsole->m_inputText.empty() )
		{
			g_DevConsole->Execute( g_DevConsole->m_inputText );
			g_DevConsole->m_inputText.clear();
		}
		return true;
	}

	if ( keyCode == KEYCODE_BACKSPACE )
	{
		if ( !g_DevConsole->m_inputText.empty() && g_DevConsole->m_insertionPointPosition > 0 )
		{
			g_DevConsole->m_insertionPointPosition--;
			g_DevConsole->m_inputText.erase(g_DevConsole->m_insertionPointPosition, 1);
		}
		return true;
	}

	if ( keyCode == KEYCODE_DELETE )
	{
		if ( !g_DevConsole->m_inputText.empty() && g_DevConsole->m_insertionPointPosition < g_DevConsole->m_inputText.size() )
		{
			g_DevConsole->m_inputText.erase( g_DevConsole->m_insertionPointPosition, 1 );
		}
		return true;
	}

	if ( keyCode == KEYCODE_ESC )
	{
		g_DevConsole->m_inputText.clear();
		g_DevConsole->m_insertionPointPosition = 0;
		return true;
	}

	if ( keyCode == KEYCODE_HOME )
	{
		g_DevConsole->m_insertionPointPosition = 0;
	}

	if ( keyCode == KEYCODE_END )
	{
		g_DevConsole->m_insertionPointPosition = static_cast< int >( g_DevConsole->m_inputText.size() );
	}

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

	if ( charCode >= 32 && charCode <= 126 && charCode != 96 )
	{
		g_DevConsole->m_insertionPointBlinkTimer->Start();
		g_DevConsole->m_insertionPointVisible = true;
		std::string first_substring = g_DevConsole->m_inputText.substr(0, g_DevConsole->m_insertionPointPosition);
		int secondHalfSize = static_cast< int >(g_DevConsole->m_inputText.size()) - g_DevConsole->m_insertionPointPosition;
		std::string second_substring = g_DevConsole->m_inputText.substr(g_DevConsole->m_insertionPointPosition, secondHalfSize);
		g_DevConsole->m_inputText = first_substring + static_cast< char >( charCode ) + second_substring;
		g_DevConsole->m_insertionPointPosition++;
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------------------------
bool DevConsole::Command_Clear( [[maybe_unused]] EventArgs& args )
{
	if ( !g_engine->m_input || g_DevConsole->m_mode == HIDDEN )
	{
		return false;
	}
	g_DevConsole->m_lines.clear();
	return true;
}

//-----------------------------------------------------------------------------------------------
bool DevConsole::Command_Help( [[maybe_unused]]  EventArgs& args )
{
	if ( !g_engine->m_input || g_DevConsole->m_mode == HIDDEN )
	{
		return false;
	}
	
	std::vector<std::string> registeredCommands = g_engine->m_eventSystem->GetAllRegisteredCommands();
	g_DevConsole->AddLine( INFO_MAJOR_COLOR, "Registered Commands", 20.f, 0.f);
	for (int commandIndex = 0; commandIndex < registeredCommands.size() ; commandIndex++)
	{
		g_DevConsole->AddLine( INFO_MAJOR_COLOR_TINT, registeredCommands[commandIndex], 20.f, 0.f);
	}
	return true;
}