#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex.hpp"

//------------------------------------------------------------------------------

constexpr int NUM_SHIP_TRIS = 5;
constexpr int NUM_SHIP_VERTS = 3 * NUM_SHIP_TRIS;
constexpr int NUM_THRUST_FLAMES = 5;
constexpr int NUM_THRUST_VERTS = 3 * NUM_THRUST_FLAMES;


//------------------------------------------------------------------------------

class PlayerShip : public Entity
{
public:
	PlayerShip(Game* owner, Vec2 const& startPos); //TODO update this to take in controller id for multiplayer
	~PlayerShip();

	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	void Die() override;

	static void InitializeLocalPlayerShipsVerts( Vertex* vertArray );

	int					m_lives = 4;
	float 				m_invincibilityTimer = 0.f;
	int					m_bulletUpgradeAmount = 0;
	bool				m_hasBulletReverse = false;
	float				m_bulletUpgradeSpeedDivisor = 1.f;

private:
	void InitializeLocalVerts();
	void UpdateFromKeyboard(float deltaSeconds);
	void BounceOffWalls();
	void Shoot();
	void Respawn();
	void RenderThrust( Vertex* vertArray );

	void UpdateFromController(float deltaSeconds);

private:
	Vertex		m_localVerts[NUM_SHIP_VERTS];
	Vertex		m_localThrustVerts[NUM_THRUST_VERTS];
	bool        m_isTurningLeft = false;
	bool        m_isTurningRight = false;
	bool        m_isThrusting = false;
	float		m_thrustFraction = -5.f;
	float		m_shootCooldownTimer;
};