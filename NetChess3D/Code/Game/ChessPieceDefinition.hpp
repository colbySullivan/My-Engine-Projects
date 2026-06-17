#pragma once
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/AABB2.hpp"
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
	void CreateGeometryForQueen();
	void CreateGeometryForRook();
	void CreateGeometryForKnight();
	void CreateGeometryForBishop();
	void CreateGeometryForPawn();

	ChessPieceType GetTypeFromString( const std::string& typeName );
	void CreateBuffersAndCopy();

public:
	ChessPieceType m_type = Count; // #todo might need to convert name to this
	std::string m_name;
	char m_symbol;
	std::string m_shaderPath;

	VertexBuffer* m_vboPlayerOne = nullptr;
	IndexBuffer* m_iboPlayerOne = nullptr;
	VertexBuffer* m_vboPlayerTwo = nullptr;
	IndexBuffer* m_iboPlayerTwo = nullptr;
	Shader* m_shader = nullptr;
	Texture* m_texturePlayerOne = nullptr;
	Texture* m_texturePlayerTwo = nullptr;
	Texture* m_normalTexturePlayerOne = nullptr;
	Texture* m_normalTexturePlayerTwo = nullptr;
	Texture* m_sgaTexturePlayerOne = nullptr;
	Texture* m_sgaTexturePlayerTwo = nullptr;
	unsigned int  m_indexCount = 0;
	std::vector<Vertex_PCUTBN> m_vertexes;
	std::vector<unsigned int> m_indexes;

	static std::map<std::string, ChessPieceDefinition> s_definitions;
};