#pragma once
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Game/ChessPiece.hpp"

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

public:
	VertexBuffer* m_vbo = nullptr;
	IndexBuffer* m_ibo = nullptr;
	unsigned int  m_indexCount = 0;
	std::vector<Vertex_PCUTBN> m_vertexes;
	std::vector<unsigned int> m_indexes;

	Vec3 m_position = Vec3::ZERO;
	float m_scale = 1.0f;
	Rgba8 m_color = Rgba8::WHITE;

	ChessPiece* m_board[8][8] = { nullptr };
private:
	void CreateBoardGeometry();
};