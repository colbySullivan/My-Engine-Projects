#pragma once
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Core/EventSystem.hpp"

struct DebugConstants
{
	float	time;
	int		debugInt;
	float	debugFloat;
	float	padding;
};

class ChessPiece;

class ChessBoard
{
public:
	ChessBoard();
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
	static bool Command_ChessMove( [[maybe_unused]] EventArgs& args );
	static bool Command_DisplayBoard( [[maybe_unused]] EventArgs& args );

public:
	// Buffers
	void CreateBuffersAndCopy();
	VertexBuffer* m_vbo = nullptr;
	IndexBuffer* m_ibo = nullptr;
	ConstantBuffer* m_debugConstant;
	unsigned int  m_indexCount = 0;

	// Debug
	DebugConstants	m_debugConstants = { };
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

	ChessPiece* m_board[8][8] = { nullptr };
private:
	void CreateBoardGeometry();
	static IntVec2 GetBoardToIntVec2( std::string chessString );
	static bool TryToDoMovePiece( std::string fromSquareString, std::string toSquareString );
	static bool MoveValidInsideBoard( IntVec2 moveSquare );
	void ChangePlayer();
};