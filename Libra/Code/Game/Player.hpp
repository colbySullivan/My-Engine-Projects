#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex.hpp"

//------------------------------------------------------------------------------
class Game;
class Texture;

//------------------------------------------------------------------------------

class Player : public Entity
{
public:
	Player(Game* owner, Vec2 const& startPos);
	~Player();

	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	virtual bool IsPlayer();
	//void Die() override;

	int					m_lives = 4;

private:
	void InitializePlayerVerts();
	void UpdateFromKeyboard(float deltaSeconds);
	//void Shoot();
	void Respawn();

	void UpdateFromController(float deltaSeconds);
	void InitializeTurretVerts();
	void RenderPlayer() const;
	void RenderTurret() const;
	void ControlOrientationTurning( float m_orientationDegrees );
	Texture* m_playerTexture = nullptr;

private:
	Vertex		m_playerVerts[NUM_PLAYER_VERTS];
	Vertex		m_turretVerts[NUM_TURRET_VERTS];
	float		m_thrustFraction = -5.f;
	bool		m_isMoving = false;
};