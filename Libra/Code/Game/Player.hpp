#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex.hpp"

//------------------------------------------------------------------------------
class Game;

//------------------------------------------------------------------------------

class Player : public Entity
{
public:
	Player(Game* owner, Vec2 const& startPos, float orientationDegrees, EntityFaction faction, Map* map, EntityType type);
	~Player();

	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	virtual bool IsPlayer() const override;

	int					m_lives = 1;
	void Respawn() override;

private:
	void InitializePlayerVerts();
	void UpdateFromKeyboard(float deltaSeconds);
	//void Shoot(); 
	void UpdateFromController(float deltaSeconds);
	void InitializeTurretVerts();
	void RenderPlayer() const;
	void RenderTurret() const;

	bool PlayerControlKeyboard();
	bool TurretControlKeyboard();
private:
	Vertex		m_playerVerts[NUM_PLAYER_VERTS];
	Vertex		m_turretVerts[NUM_TURRET_VERTS];
	float		m_thrustFraction = -5.f;
	bool		m_isMoving = false;
};