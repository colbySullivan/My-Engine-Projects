#pragma once
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"

class ChessBoard
{
public:
	ChessBoard();
	~ChessBoard();
	virtual void Update();
	virtual void Render() const;

	void CreateBuffersAndCopy();
public:
	VertexBuffer* m_vbo = nullptr;
	IndexBuffer* m_ibo = nullptr;
	unsigned int  m_indexCount = 0;
	std::vector<Vertex_PCUTBN> m_vertexes;
	std::vector<unsigned int> m_indexes;

	Vec3 m_position = Vec3::ZERO;
	float m_scale = 1.0f;
	Rgba8 m_color = Rgba8::WHITE;
private:
	void CreateBoardGeometry();
};