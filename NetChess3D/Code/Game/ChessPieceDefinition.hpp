#pragma once
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include <string>
#include <map>


enum ChessPieceType
{
	Pawn, 
	Rook, 
	Knight, 
	Bishop, 
	Queen, 
	King,
	Count
};

//-----------------------------------------------------------------------------------------------
class ChessPieceDefinition
{
public:
	ChessPieceDefinition() = default;
	~ChessPieceDefinition();

	static void InitializeChessPieceDefs();
	static const ChessPieceDefinition* GetByName( const std::string& name );
	static void LoadPiecesDefsFromFile( char const* filePath );

	void CreateGeometryForKing();

public:
	ChessPieceType m_type = Count; // #todo might need to convert name to this
	std::string m_name;
	char m_symbol;

	VertexBuffer* m_vbo;
	IndexBuffer* m_ibo;
	unsigned int  m_indexCount = 0;
	std::vector<Vertex_PCUTBN> m_vertexes;

	static std::map<std::string, ChessPieceDefinition> s_definitions;
};