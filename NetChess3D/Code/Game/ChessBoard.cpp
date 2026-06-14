#include "Game/ChessBoard.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Game/ChessPiece.hpp"

static ChessBoard* g_activeChessBoard = nullptr;

//-----------------------------------------------------------------------------------------------
ChessBoard::ChessBoard( Game* game )
	: m_game( game )
{
	for ( int row = 0; row < 8; ++row )
	{
		for ( int col = 0; col < 8; ++col )
		{
			m_board[row][col] = nullptr;
		}
	}
	g_activeChessBoard = this;
	m_gameOver = false;
	SubscribeEventCallbackFunction( "ChessMove", Command_ChessMove );
	SubscribeEventCallbackFunction( "DisplayBoard", Command_DisplayBoard );
	SubscribeEventCallbackFunction( "ChessOverride", Command_ChessOverride );

	CreateBoardGeometry();
	CreateBuffersAndCopy();
	m_texture = g_engine->m_render->CreateTextureFromImage( "Data/Textures/woodfloor_d.png" );
	m_normalTexture = g_engine->m_render->CreateTextureFromImage( "Data/Textures/woodfloor_n.png" );
}

//-----------------------------------------------------------------------------------------------
ChessBoard::~ChessBoard()
{
	UnsubscribeEventCallbackFunction( "ChessMove", Command_ChessMove );
	UnsubscribeEventCallbackFunction( "DisplayBoard", Command_DisplayBoard );
	UnsubscribeEventCallbackFunction( "ChessOverride", Command_ChessOverride );

	if ( g_activeChessBoard == this )
	{
		g_activeChessBoard = nullptr;
	}

	delete m_vbo;
	m_vbo = nullptr;
	delete m_ibo;
	m_ibo = nullptr;

	m_vertexes.clear();
}

//-----------------------------------------------------------------------------------------------
void ChessBoard::Update()
{
	UpdateDebugConstants();
}

//-----------------------------------------------------------------------------------------------
void ChessBoard::Render() const
{
	Mat44 modelMatrix = Mat44::MakeTranslation3D( m_position );
	g_engine->m_render->SetModelConstants( modelMatrix, m_color );
	g_engine->m_render->m_desiredBlendMode = BlendMode::OPAQUE;
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	unsigned int indexCount = ( unsigned int )m_indexes.size();
	g_engine->m_render->BindTexture(m_texture, 0);
	g_engine->m_render->BindTexture(m_normalTexture, 1);
	SetDebugConstant();
	g_engine->m_render->DrawIndexBuffer( m_vbo, m_ibo, indexCount );

}

//-----------------------------------------------------------------------------------------------
void ChessBoard::CreateBuffersAndCopy()
{
	m_debugConstant = g_engine->m_render->CreateConstantBuffer( sizeof( DebugConstants ) );

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
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "    A  B  C  D  E  F  G  H" );
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
				if ( piece->m_playernum == 2 )
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
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "    A  B  C  D  E  F  G  H" );
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

void ChessBoard::UpdateDebugConstants()
{
	m_debugConstants.time = (float)g_engine->m_systemClock->GetTotalSeconds();
	m_debugConstants.debugFloat = m_debugFloat;
	m_debugConstants.debugInt = m_debugInt;
}

//-----------------------------------------------------------------------------------------------
void ChessBoard::SetDebugConstant() const
{
	g_engine->m_render->BindConstantBuffer( 1, m_debugConstant );
	g_engine->m_render->CopyCPUToGPU( &m_debugConstants, sizeof( DebugConstants ), m_debugConstant );
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
	if ( piece )
	{
		piece->m_position = GetWorldPositionFromSquare( IntVec2( col, row ) );
	}
}

//------------------------------------------------------------------------------
bool ChessBoard::IsValidTargetSquare( IntVec2 const& toSquare, int currentPlayer ) const
{
	ChessPiece* targetPiece = GetPieceAt( toSquare.y, toSquare.x );
	if ( targetPiece != nullptr && targetPiece->m_playernum == currentPlayer )
	{
		return false;
	}
	return true;
}

//------------------------------------------------------------------------------
bool ChessBoard::IsPathClear( IntVec2 const& fromSquare, IntVec2 const& toSquare ) const
{
	int manhattanX = toSquare.x - fromSquare.x;
	int manhattanY = toSquare.y - fromSquare.y;

	int stepX = 0;
	int stepY = 0;
	if ( manhattanX != 0 )
	{
		stepX = manhattanX > 0 ? 1 : -1;
	}
	if ( manhattanY != 0 )
	{
		stepY = manhattanY > 0 ? 1 : -1;
	}

	int currentX = fromSquare.x + stepX;
	int currentY = fromSquare.y + stepY;

	while ( currentX != toSquare.x || currentY != toSquare.y )
	{
		if ( GetPieceAt( currentY, currentX ) != nullptr )
		{
			return false;
		}
		currentX += stepX;
		currentY += stepY;
	}

	return true;
}

//------------------------------------------------------------------------------
bool ChessBoard::IsValidKnightMove( IntVec2 const& fromSquare, IntVec2 const& toSquare ) const
{
	int manhattanX = abs( toSquare.x - fromSquare.x );
	int manhattanY = abs( toSquare.y - fromSquare.y );

	return ( manhattanX * manhattanY == 2 );
}

bool ChessBoard::isDiaganol( IntVec2 const& fromSquare, IntVec2 const& toSquare ) const
{
	int manhattanX = abs( toSquare.x - fromSquare.x );
	int manhattanY = abs( toSquare.y - fromSquare.y );

	return ( manhattanX != 0 && manhattanX == manhattanY );
}

//-----------------------------------------------------------------------------------------------
void ChessBoard::SetBoardFromString( std::string boardString )
{
	if ( boardString.length() < 64 )
	{
		g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, "SetBoardFromString: Board string must be at least 64 characters long!" );
		return;
	}

	for ( int row = 0; row < 8; ++row )
	{
		for ( int column = 0; column < 8; ++column )
		{
			ChessPiece* lingeringPiece = GetPieceAt( row, column );
			if ( lingeringPiece )
			{
				m_game->RemoveChessPiece( lingeringPiece );
				SetPieceAt( row, column, nullptr );
				delete lingeringPiece;
			}

			int stringPosition = ( row * 8 ) + column;
			int playerNum = 1;
			char pieceChar = boardString[stringPosition];
			if ( pieceChar == '.' )
			{
				continue;
			}

			std::string defName = GetTypeFromChar( pieceChar, playerNum );
			ChessPieceDefinition const* pieceDef = ChessPieceDefinition::GetByName( defName );

			if ( pieceDef )
			{
				Vec3 position = GetWorldPositionFromSquare( IntVec2( column, row ) );
				ChessPiece* newPiece = new ChessPiece( m_game, pieceDef, position, playerNum );
				SetPieceAt( row, column, newPiece );
				m_game->m_chessPieces.push_back( newPiece );
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void ChessBoard::CreateBoardGeometry()
{
	Vec3 bl = Vec3( 0.f, 0.f, 0.f );
	Vec3 br = Vec3( 1.f, 0.f, 0.f );
	Vec3 tr = Vec3( 1.f, 1.f, 0.f );
	Vec3 tl = Vec3( 0.f, 1.f, 0.f );
	for ( int row = 0; row < 8; ++row )
	{
		for ( int column = 0; column < 8; ++column )
		{
			Rgba8 color = Rgba8( 120, 120, 120, 255 );
			if ( ( row + column ) % 2 == 0 )
			{
				color = Rgba8( 180, 180, 180, 255 );
			}
			bl = Vec3( 0.f + column, 0.f + row, 0.f );
			br = Vec3( 1.f + column, 0.f + row, 0.f );
			tr = Vec3( 1.f + column, 1.f + row, 0.f );
			tl = Vec3( 0.f + column, 1.f + row, 0.f );
			AddVertsForQuad3D( m_vertexes, m_indexes, bl, br, tr, tl, color, AABB2::ZERO_TO_ONE );
		}
	}
	AABB3 boardBox = AABB3( Vec3( -0.3f, -0.3f, -1.f ), Vec3( tr.x + 0.3f, tr.y + 0.3f, -.01f ) );
	AddVertsForAABB3D( m_vertexes, m_indexes, boardBox, Rgba8( 75, 75, 75 ), AABB2::ZERO_TO_ONE);
}

//-----------------------------------------------------------------------------------------------
IntVec2 ChessBoard::GetBoardToIntVec2( std::string chessString )
{
	char firstChar = chessString[0] - 97;
	int secondNumber = chessString[1] - '0' - 1;
	return IntVec2( (int)firstChar, secondNumber );
}

//-----------------------------------------------------------------------------------------------
bool ChessBoard::TryToDoMovePiece( std::string fromSquareString, std::string toSquareString, bool teleport )
{
	IntVec2 fromSquare = GetBoardToIntVec2( fromSquareString );
	IntVec2 toSquare = GetBoardToIntVec2( toSquareString );

	if ( g_activeChessBoard->m_gameOver )
	{
		g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, "ChessMove: Game over use the ChessBegin command to restart" );
		return false;
	}

	// Missing one or both to / from
	if ( fromSquareString.empty() || toSquareString.empty() )
	{
		g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, "ChessMove: requires 'from' and 'to' parameter (e.g., ChessMove from=a2 to=a3)" );
		return false;
	}

	// Tried to move outside of board
	if ( !MoveValidInsideBoard( fromSquare ) || !MoveValidInsideBoard( toSquare ) )
	{
		g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, "ChessMove: move invalid please ensure it is within the board" );
		return false;
	}

	ChessPiece* piece = g_activeChessBoard->GetPieceAt( fromSquare.y, fromSquare.x );

	// No piece at to location
	if ( piece == nullptr )
	{
		g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, Stringf( "No piece at %s", fromSquareString.c_str() ) );
		return false;
	}

	// Wrong player piece
	if ( piece->m_playernum != g_activeChessBoard->m_currentPlayerNum )
	{
		g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, Stringf( "Piece at 'from' square is not player %i", g_activeChessBoard->m_currentPlayerNum ) );
		return false;
	}

	// No friendly fire
	if ( !g_activeChessBoard->IsValidTargetSquare( toSquare, piece->m_playernum ) )
	{
		g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, "ChessMove: Cannot capture your own piece!" );
		return false;
	}

	// semi-legal moves
	if ( !teleport )
	{
		// Ensure L movement
		if ( piece->m_definition->m_type == Knight )
		{
			if ( !g_activeChessBoard->IsValidKnightMove( fromSquare, toSquare ) )
			{
				g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, "ChessMove: Knights must move in an L-shape!" );
				return false;
			}
		}
	
		// "Sliding" pieces path check
		if ( piece->m_definition->m_type != Knight )
		{
			if ( !g_activeChessBoard->IsPathClear( fromSquare, toSquare ) )
			{
				g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, "ChessMove: Path is blocked by another piece!" );
				return false;
			}
			bool movingDiagonal = g_activeChessBoard->isDiaganol( fromSquare, toSquare );
			if ( piece->m_definition->m_type == Rook && movingDiagonal )
			{
				g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, "ChessMove: Rooks can only move in a straight line!" );
				return false;
			}
		}

		// Pawn movement
		if ( piece->m_definition->m_type == Pawn )
		{
			int manhattanX = toSquare.x - fromSquare.x;
			int manhattanY = toSquare.y - fromSquare.y;

			ChessPiece* targetPiece = g_activeChessBoard->GetPieceAt( toSquare.y, toSquare.x );

			// Can only move 2 on first move
			if ( abs( manhattanY ) > 1 )
			{
				if ( (fromSquare.y != 1 && fromSquare.y != 6) || abs( manhattanY ) > 2 )
				{
					g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, "ChessMove: Pawns can only move 2 spaces on their first turn!" );
					return false;
				}
			}

			// Moving forward not empty
			if ( manhattanX == 0 && targetPiece != nullptr )
			{
				g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, "ChessMove: Pawns cannot capture forward!" );
				return false;
			}

			// Diagonal capture check
			if ( manhattanX != 0 && targetPiece == nullptr )
			{
				g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, "ChessMove: Pawns can only move diagonally to capture!" );
				return false;
			}
		}

		// King movement
		if ( piece->m_definition->m_type == King )
		{
			int manhattanX = toSquare.x - fromSquare.x;
			int manhattanY = toSquare.y - fromSquare.y;

			// king adjacent check
			if ( g_activeChessBoard->IsKingAdjacentToAnotherKing( toSquare, piece->m_playernum ) )
			{
				g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, "ChessMove: Kings cannot be adjacent to each other!" );
				return false;
			}

			if ( manhattanX > 1 || manhattanY > 1 )
			{
				if ( manhattanX == 2 && manhattanY == 0 && g_activeChessBoard->TryExecuteCastling( fromSquare, toSquare, piece ) )
				{
					g_activeChessBoard->ChangePlayer( piece );
					return false;
				}
				else
				{
					g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, "ChessMove: Kings can only move 1 square in any direction!" );
					return false;
				}
			}
		}

		// Rook, Bishop, Queen movement
		if ( piece->m_definition->m_type == Rook || piece->m_definition->m_type == Bishop || piece->m_definition->m_type == Queen )
	{
		int manhattanX = abs( toSquare.x - fromSquare.x );
		int manhattanY = abs( toSquare.y - fromSquare.y );

		// No movement
		if ( manhattanX == 0 && manhattanY == 0 )
		{
			g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, "ChessMove: Destination square is the same as origin!" );
			return false;
		}

		// Rook Movement
		if ( piece->m_definition->m_type == Rook )
		{
			if ( manhattanX != 0 && manhattanY != 0 )
			{
				g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, "ChessMove: Rooks cannot move diagonally!" );
				return false;
			}
		}
		// Bishop movement
		else if ( piece->m_definition->m_type == Bishop )
		{
			if ( manhattanX == 0 || manhattanX != manhattanY )
			{
				g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, "ChessMove: Bishops must move diagonally!" );
				return false;
			}
		}
		
		// Queen movement
		else 
		{
			if ( !( manhattanX == manhattanY || manhattanX == 0 || manhattanY == 0 ) )
			{
				g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, "ChessMove: Queens move like Rooks or Bishops (straight or diagonal)!" );
				return false;
			}
		}
	}
	}
	ChessPiece* toPiece = g_activeChessBoard->GetPieceAt( toSquare.y, toSquare.x );
	ChessPieceType capturedType = Count;

	// Move pieces in console
	g_activeChessBoard->m_board[fromSquare.y][fromSquare.x] = nullptr;
	g_activeChessBoard->m_board[toSquare.y][toSquare.x] = piece;

	// Move pieces on board
	ChessPiece::MoveStyle movesyle = ( piece->m_definition->m_type == Knight ) ? ChessPiece::MoveStyle::Hop : ChessPiece::MoveStyle::Slide;
	piece->StartMove( g_activeChessBoard->GetWorldPositionFromSquare(toSquare), movesyle );

	piece->m_firstMove = false;	
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, Stringf( "Moved piece from %s to %s", fromSquareString.c_str(), toSquareString.c_str() ) );

	// Capture piece
	if ( toPiece ) 
	{
		capturedType = toPiece->m_definition->m_type;
		piece->m_game->RemoveChessPiece( toPiece );
		if ( toPiece && capturedType == King )
		{
			g_activeChessBoard->m_gameOver = true;
			piece->m_game->ChangePlayerCamera( 3 ); // Change to fly over
			g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, Stringf( "Player %i won", g_activeChessBoard->m_currentPlayerNum ) );
		}
		delete toPiece;
		toPiece = nullptr;
	}

	// Switch player
	g_activeChessBoard->ChangePlayer( piece );

	return false;
}

//-----------------------------------------------------------------------------------------------
bool ChessBoard::MoveValidInsideBoard( IntVec2 moveSquare )
{
	if ( moveSquare.x < 0 || moveSquare.x >= 8 || moveSquare.y < 0 || moveSquare.y >= 8 )
	{
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------------------------
bool ChessBoard::IsKingAdjacentToAnotherKing( IntVec2 const& kingPosition, int kingPlayer ) const
{
	std::vector<IntVec2> adjacentSquares;
	adjacentSquares.push_back(IntVec2( -1, -1 )); //topLeft
	adjacentSquares.push_back(IntVec2( -1, 0 )); //topCenter
	adjacentSquares.push_back(IntVec2( -1, 1 )); //topRight

	adjacentSquares.push_back(IntVec2( 0, -1 )); //leftCenter
	adjacentSquares.push_back(IntVec2( 0, 1 )); //rightCenter

	adjacentSquares.push_back(IntVec2( 1, -1 )); //bottomLeft
	adjacentSquares.push_back(IntVec2( 1, 0 )); //bottomCenter
	adjacentSquares.push_back(IntVec2( 1, 1 )); //bottomRight

	for ( IntVec2 squareOffset : adjacentSquares )
	{
		IntVec2 checkSquare = kingPosition + squareOffset;

		ChessPiece* adjacentPiece = GetPieceAt( checkSquare.y, checkSquare.x );
		if ( adjacentPiece != nullptr && adjacentPiece->m_definition->m_type == King && adjacentPiece->m_playernum != kingPlayer )
		{
			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------------------------
void ChessBoard::ChangePlayer( ChessPiece* piece )
{
	m_currentPlayerNum = ( m_currentPlayerNum % 2 ) + 1;
	if ( !g_activeChessBoard->m_gameOver )
	{
		piece->m_game->ChangePlayerCamera( g_activeChessBoard->m_currentPlayerNum );
		g_engine->m_console->AddLine( DevConsole::INFO_MINOR_COLOR, "" );
		g_engine->m_console->AddLine( DevConsole::INFO_MINOR_COLOR, Stringf( "Player %i turn", g_activeChessBoard->m_currentPlayerNum ) );

		g_activeChessBoard->PrintBoardStateToConsole();
	}
}

//-----------------------------------------------------------------------------------------------
std::string ChessBoard::GetTypeFromChar( const char& typeName, int& playerNum )
{
	char typeNameUpper = ( char )toupper( typeName );
	if ( typeName != typeNameUpper )
	{
		playerNum = 2;
	}

	if ( typeNameUpper == 'P' )   return "Pawn";
	if ( typeNameUpper == 'R' )   return "Rook";
	if ( typeNameUpper == 'N' ) return "Knight";
	if ( typeNameUpper == 'B' ) return "Bishop";
	if ( typeNameUpper == 'Q' )  return "Queen";
	if ( typeNameUpper == 'K' )   return "King";

	return "";
}

//-----------------------------------------------------------------------------------------------
bool ChessBoard::TryExecuteCastling( IntVec2 const& fromSquare, IntVec2 const& toSquare, ChessPiece* king )
{
	if ( !king->m_firstMove )
	{
		g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, "ChessMove: Cannot castle because the King has already moved!" );
		return false;
	}

	// #todo will do check detection here next assignment

	bool isKingside = ( toSquare.x > fromSquare.x );
	int rookStartCol = isKingside ? 7 : 0;
	int rookEndCol = isKingside ? 5 : 3;

	ChessPiece* rook = GetPieceAt( fromSquare.y, rookStartCol );
	if ( rook == nullptr || rook->m_definition->m_type != Rook )
	{
		g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, "ChessMove: Matching Rook not found on starting square!" );
		return false;
	}
	if ( !rook->m_firstMove )
	{
		g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, "ChessMove: Cannot castle because this Rook has already moved!" );
		return false;
	}

	SetPieceAt( fromSquare.y, fromSquare.x, nullptr );
	SetPieceAt( toSquare.y, toSquare.x, king );
	king->m_firstMove = false;

	SetPieceAt( fromSquare.y, rookStartCol, nullptr );
	SetPieceAt( fromSquare.y, rookEndCol, rook );
	rook->m_firstMove = false;

	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "Castling executed successfully!" );

	return true;
}

//-----------------------------------------------------------------------------------------------
bool ChessBoard::Command_ChessMove( EventArgs& args )
{
	if ( g_activeChessBoard )
	{
		std::string fromSquareString = args.GetValue( "from", "" );
		std::string toSquareString = args.GetValue( "to", "" );
		bool teleport = args.GetValue( "teleport", false );
		return TryToDoMovePiece( fromSquareString, toSquareString, teleport );
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

//-----------------------------------------------------------------------------------------------
bool ChessBoard::Command_ChessOverride( [[maybe_unused]] EventArgs& args )
{
	if ( g_activeChessBoard )
	{
		std::string boardString = args.GetValue( "board", "" );
		g_activeChessBoard->SetBoardFromString( boardString );
		g_activeChessBoard->PrintBoardStateToConsole();
	}
	return false;
}