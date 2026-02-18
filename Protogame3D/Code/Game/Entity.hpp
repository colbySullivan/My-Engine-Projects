#pragma once
#include <Engine/Math/Vec2.hpp>
#include "Game/Game.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


//-----------------------------------------------------------------------------------------------

class RandomNumberGenerator;

//-----------------------------------------------------------------------------------------------
class Entity
{
public:
	Entity(Game* owner, Vec2 const& startPos);
	virtual ~Entity();

	virtual void Update(float deltaSeconds) = 0;
	virtual void Render() const = 0;
	virtual void DebugRender() const;
	virtual void Die();

	bool        IsOffscreen() const;
	Vec2        GetForwardNormal() const;
	void		WrapAroundScreen();
	void		spawnRandomEdge();
	void		HealthVisual();

	RandomNumberGenerator g_rng;

public:
	Game*		m_game = nullptr;
	Vec2        m_position;
	Vec2        m_velocity;
	float       m_orientationDegrees = 0.f;  // counter-clockwise from +x/east
	float       m_angularVelocity = 0.f;  // (signed) spin rate, in degrees per second, + is counter-clockwise
	float       m_physicsRadius = 5.f;
	float       m_cosmeticRadius = 10.f;
	float		m_debrisSize = 0.5f;
	int         m_health = 1;
	int			m_startingHealth = 1;
	int			m_debrisAmount = 3;
	bool        m_isDead = false;  // gameplay idea
	bool        m_isGarbage = false;  // code idea
	Rgba8		m_entityColor = Rgba8(200, 200, 200, 127);
	Rgba8		m_startingEntityColor = Rgba8(200, 200, 200, 127);

};