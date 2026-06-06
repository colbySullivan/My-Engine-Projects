#pragma once
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Core/EventSystem.hpp"

class ChessPiece;

class ChessBoard
{
public:
	ChessBoard();
	~ChessBoard();
	virtual void Update();
	virtual void Render() const;

	void CreateBuffersAndCopy();
	void PrintBoardStateToConsole() const;
	IntVec2 GetSquareFromWorldPosition( Vec3 const& worldPos ) const;
	Vec3 GetWorldPositionFromSquare( IntVec2 const& square ) const;

	ChessPiece* GetPieceAt( int row, int col ) const;
	void SetPieceAt( int row, int col, ChessPiece* piece );
	static bool Command_ChessMove( [[maybe_unused]] EventArgs& args );
	static bool Command_DisplayBoard( [[maybe_unused]] EventArgs& args );

public:
	VertexBuffer* m_vbo = nullptr;
	IndexBuffer* m_ibo = nullptr;
	Texture* m_texture = nullptr;
	unsigned int  m_indexCount = 0;
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