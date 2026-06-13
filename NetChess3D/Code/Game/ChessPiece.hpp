#pragma once
#include "Game/Game.hpp"
#include "Game/ChessPieceDefinition.hpp"
#include "Engine/Core/Timer.hpp"

class Game;

//-----------------------------------------------------------------------------------------------
class ChessPiece
{
public:
	ChessPiece( Game* owner, ChessPieceDefinition const* definition, Vec3 const& position, int playernum );
	virtual ~ChessPiece();

	enum class MoveStyle
	{
		Instant,
		Slide,
		Hop
	};

	virtual void Update();
	virtual void Render() const;

	void StartMove( Vec3 const& targetWorldPosition, MoveStyle style, float durationSeconds = 0.5f );

public:
	Game* m_game;
	ChessPieceDefinition const* m_definition = nullptr;
	Vec3 m_position = Vec3::ZERO;
	float m_scale = 1.0f;
	Rgba8 m_color = Rgba8::WHITE;
	int m_playernum;
	bool m_firstMove = true;
	Vec3 m_moveStart = Vec3::ZERO;
	Vec3 m_moveEnd = Vec3::ZERO;
	Timer* m_moveTimer = nullptr;
	MoveStyle m_moveStyle = MoveStyle::Instant;
};