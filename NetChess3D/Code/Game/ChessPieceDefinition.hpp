#pragma once
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
	~ChessPieceDefinition() = default;

	static void InitializeChessPieceDefs();
	static const ChessPieceDefinition* GetByName( const std::string& name );

public:
	ChessPieceType m_type = Count; // #todo might need to convert name to this
	std::string m_name;
	char m_symbol;

	static std::map<std::string, ChessPieceDefinition> s_definitions;
};