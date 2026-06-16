#pragma once
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "ChessPieceDefinition.hpp"

struct DebugConstants
{
	float	time;
	int		debugInt;
	float	debugFloat;
	float	padding;
};

class ChessPiece;
class Game;

class ChessBoard
{
public:
	ChessBoard( Game* game );
	~ChessBoard();
	virtual void Update();
	virtual void Render() const;

	void PrintBoardStateToConsole() const;
	IntVec2 GetSquareFromWorldPosition( Vec3 const& worldPos ) const;
	Vec3 GetWorldPositionFromSquare( IntVec2 const& square ) const;
	void UpdateDebugConstants();
	void SetDebugConstant() const;
	ChessPiece* GetPieceAt( int row, int col ) const;
	void SetPieceAt( int row, int col, ChessPiece* piece );
	void GetImpactedPieceOrSquare( Vec3 const& rayStart, Vec3 const& rayDirection, float maxDistance, ChessPiece*& outPiece, IntVec2& outSquare ) const;
	bool IsValidTargetSquare( IntVec2 const& toSquare, int currentPlayer ) const;
	bool IsPathClear( IntVec2 const& fromSquare, IntVec2 const& toSquare ) const;
	bool IsValidKnightMove( IntVec2 const& fromSquare, IntVec2 const& toSquare ) const;
	bool isDiaganol( IntVec2 const& fromSquare, IntVec2 const& toSquare ) const;
	void SetBoardFromString( std::string boardString );

	static bool Command_ChessMove( [[maybe_unused]] EventArgs& args );
	static bool Command_DisplayBoard( [[maybe_unused]] EventArgs& args );
	static bool Command_ChessOverride( [[maybe_unused]] EventArgs& args );

public:
	// Buffers
	void CreateBuffersAndCopy();
	VertexBuffer* m_vbo = nullptr;
	IndexBuffer* m_ibo = nullptr;
	ConstantBuffer* m_debugConstant;
	unsigned int  m_indexCount = 0;
	Shader* m_shader = nullptr;
	Game* m_game = nullptr;

	// Debug
	DebugConstants	m_debugConstantValues = { };
	int				m_debugInt = 0;
	float			m_debugFloat = 0.f;

	Texture* m_texture = nullptr;
	Texture* m_normalTexture = nullptr;
	std::vector<Vertex_PCUTBN> m_vertexes;
	std::vector<unsigned int> m_indexes;

	Vec3 m_position = Vec3::ZERO;
	float m_scale = 1.0f;
	Rgba8 m_color = Rgba8::WHITE;
	int m_currentPlayerNum = 1;
	bool m_gameOver = false;

	// En Passant tracking
	IntVec2 m_lastPawnMoveFrom = IntVec2( -1, -1 );
	IntVec2 m_lastPawnMoveTo = IntVec2( -1, -1 );

	// Moves
	ChessPiece* m_selectedPiece = nullptr;
	bool m_tryAllMovesRequested;

	ChessPiece* m_board[8][8] = { nullptr };
private:
	void CreateBoardGeometry();
	static IntVec2 GetBoardToIntVec2( std::string chessString );
	std::string GetIntVec2ToString( IntVec2 boardPosition );
	static bool CheckBoardSquareValid( IntVec2 fromSquare, IntVec2 toSquare, bool teleport = false, std::string promoteTo = "" );
	static bool MovePiece( std::string fromSquareString, std::string toSquareString, bool teleport = false, std::string promoteTo = "" );
	static bool MoveValidInsideBoard( IntVec2 moveSquare );
	bool IsKingAdjacentToAnotherKing( IntVec2 const& kingPosition, int kingPlayer ) const;
	ChessPieceType GetPieceTypeFromString( std::string const& typeName ) const;
	bool IsPawnPromotionRow( int row, int playerNum ) const;
	bool PromotePawn( ChessPiece* pawn, ChessPieceType promotionType );
	void ChangePlayer( ChessPiece* piece );
	std::string GetTypeFromChar( const char& typeName, int& playerNum );
	bool TryExecuteCastling( IntVec2 const& fromSquare, IntVec2 const& toSquare, ChessPiece* king );
	bool IsValidEnPassantMove( IntVec2 const& fromSquare, IntVec2 const& toSquare, ChessPiece* pawn ) const;
	bool TryExecuteEnPassant();
	void TryAllMoves();
};