#include "Game/ChessBoard.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"

static ChessBoard* g_activeChessBoard = nullptr;

//-----------------------------------------------------------------------------------------------
ChessBoard::ChessBoard()
{
	for ( int row = 0; row < 8; ++row )
	{
		for ( int col = 0; col < 8; ++col )
		{
			m_board[row][col] = nullptr;
		}
	}
	g_activeChessBoard = this;
	SubscribeEventCallbackFunction( "ChessMove", Command_ChessMove );
	SubscribeEventCallbackFunction( "DisplayBoard", Command_DisplayBoard );
	CreateBoardGeometry();
	CreateBuffersAndCopy();
}

//-----------------------------------------------------------------------------------------------
ChessBoard::~ChessBoard()
{
	delete m_vbo;
	m_vbo = nullptr;
	delete m_ibo;
	m_ibo = nullptr;

	m_vertexes.clear();
}

//-----------------------------------------------------------------------------------------------
void ChessBoard::Update()
{

}

//-----------------------------------------------------------------------------------------------
void ChessBoard::Render() const
{
	Mat44 modelMatrix = Mat44::MakeTranslation3D( m_position );
	g_engine->m_render->SetModelConstants( modelMatrix, m_color );
	g_engine->m_render->m_desiredBlendMode = BlendMode::OPAQUE;
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	unsigned int indexCount = ( unsigned int )m_indexes.size();
	g_engine->m_render->DrawIndexBuffer( m_vbo, m_ibo, indexCount );
}

//-----------------------------------------------------------------------------------------------
void ChessBoard::CreateBuffersAndCopy()
{
	unsigned int vertexBufferSize = ( unsigned int )( m_vertexes.size() * sizeof( Vertex_PCUTBN ) );
	unsigned int vertexStride = ( unsigned int )sizeof( Vertex_PCUTBN );
	m_vbo = g_engine->m_render->CreateVertexBuffer( vertexBufferSize, vertexStride );

	unsigned int indexBufferSize = ( unsigned int )( m_indexes.size() * sizeof( unsigned int ) );
	m_ibo = g_engine->m_render->CreateIndexBuffer( indexBufferSize );

	g_engine->m_render->CopyCPUToGPU( m_vertexes.data(), vertexBufferSize, m_vbo );
	g_engine->m_render->CopyCPUToGPU( m_indexes.data(), indexBufferSize, m_ibo );
}

//-----------------------------------------------------------------------------------------------
void ChessBoard::PrintBoardStateToConsole() const
{
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "   A  B  C  D  E  F  G  H" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "  ------------------------" );

	for ( int row = 7; row >= 0; --row )
	{
		std::string rowString = Stringf( "%d |", row + 1 );

		for ( int col = 0; col < 8; ++col )
		{
			ChessPiece* piece = m_board[row][col];
			if ( piece && piece->m_definition )
			{
				char symbol = piece->m_definition->m_symbol;
				if ( piece->m_playernum == PlayerTwo )
				{
					symbol += 32;
				}
				rowString += Stringf( " %c ", symbol );
			}
			else
			{
				rowString += " . ";
			}
		}
		rowString += Stringf( "| %d", row + 1 );
		g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, rowString );
	}

	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "  ------------------------" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "   A  B  C  D  E  F  G  H" );
}
//-----------------------------------------------------------------------------------------------
IntVec2 ChessBoard::GetSquareFromWorldPosition( Vec3 const& worldPos ) const
{
	Vec3 localPos = worldPos - m_position;
	int col = ( int )localPos.x;
	int row = ( int )localPos.y;
	return IntVec2( col, row );	// #todo might need to swtich these
}

//-----------------------------------------------------------------------------------------------
Vec3 ChessBoard::GetWorldPositionFromSquare( IntVec2 const& square ) const
{
	return m_position + Vec3( square.x + 0.5f, square.y + 0.5f, 0.f );
}

//-----------------------------------------------------------------------------------------------
ChessPiece* ChessBoard::GetPieceAt( int row, int col ) const
{
	if ( row < 0 || row >= 8 || col < 0 || col >= 8 )
	{
		return nullptr;
	}
	return m_board[row][col];
}

//-----------------------------------------------------------------------------------------------
void ChessBoard::SetPieceAt( int row, int col, ChessPiece* piece )
{
	if ( row < 0 || row >= 8 || col < 0 || col >= 8 )
	{
		return;
	}
	m_board[row][col] = piece;
}

//-----------------------------------------------------------------------------------------------
void ChessBoard::CreateBoardGeometry()
{
	Vec3 bl = Vec3( 0.f, 0.f, 0.f );
	Vec3 br = Vec3( 1.f, 0.f, 0.f );
	Vec3 tr = Vec3( 1.f, 1.f, 0.f );
	Vec3 tl = Vec3( 0.f, 1.f, 0.f );
	for (int row = 0; row < 8 ; ++row)
	{
		for ( int column = 0; column < 8; ++column )
		{
			Rgba8 color = Rgba8( 120, 120, 255, 255 );
			if ( ( row + column ) % 2 == 0 )
			{
				color = Rgba8( 120, 255, 120, 120 );
			}
			bl = Vec3( 0.f + row, 0.f + column, 0.f );
			br = Vec3( 1.f + row, 0.f + column, 0.f );
			tr = Vec3( 1.f + row, 1.f + column, 0.f );
			tl = Vec3( 0.f + row, 1.f + column, 0.f );
			AddVertsForQuad3D( m_vertexes, m_indexes, bl, br, tr, tl, color, AABB2::ZERO_TO_ONE );
		}
	}
}

//-----------------------------------------------------------------------------------------------
bool ChessBoard::Command_ChessMove( [[maybe_unused]] EventArgs& args )
{
	if ( g_activeChessBoard )
	{
		std::string toSquare = args.GetValue( "to", "" );
		if ( toSquare.empty() )
		{
			g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, "ChessMove requires 'to' parameter (e.g., ChessMove to=a7)" );
			return false;
		}

		//g_activeChessBoard->GetPieceAt( )
	}
	return false;
}	

//-----------------------------------------------------------------------------------------------
bool ChessBoard::Command_DisplayBoard( [[maybe_unused]] EventArgs& args )
{
	if ( g_activeChessBoard )
	{
		g_activeChessBoard->PrintBoardStateToConsole();
	}
	return false;
}