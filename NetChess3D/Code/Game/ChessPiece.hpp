#pragma once
#include "Game/Game.hpp"
#include "Game/ChessPieceDefinition.hpp"

class Game;

//-----------------------------------------------------------------------------------------------
class ChessPiece
{
public:
	ChessPiece( Game* owner, ChessPieceDefinition const* definition, Vec3 const& position, int playernum );
	virtual ~ChessPiece();

	virtual void Update();
	virtual void Render() const;
public:
	Game* m_game;
	ChessPieceDefinition const* m_definition = nullptr;
	Vec3 m_position = Vec3::ZERO;
	float m_scale = 1.0f;
	Rgba8 m_color = Rgba8::WHITE;
	int m_playernum;
};