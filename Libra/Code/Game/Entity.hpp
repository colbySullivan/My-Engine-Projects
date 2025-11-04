#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


//-----------------------------------------------------------------------------------------------

class RandomNumberGenerator;
class Game;

//-----------------------------------------------------------------------------------------------
enum EntityType
{
	ENTITY_TYPE_GOOD_PLAYER,
	ENTITY_TYPE_EVIL_SCORPIO,
	ENTITY_TYPE_EVIL_LEO,
	ENTITY_TYPE_EVIL_ARIES,
	ENTITY_TYPE_GOOD_BULLET,
	ENTITY_TYPE_EVIL_BULLET,
	NUM_ENTITY_TYPES
};

//-----------------------------------------------------------------------------------------------
//typedef std::vector<Entity*> EntityList;

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
	virtual bool IsPlayer();

	bool        IsOffscreen() const;
	Vec2        GetForwardNormal() const;
	void		WrapAroundScreen();
	void		spawnRandomEdge();

	RandomNumberGenerator g_rng;

public:
	Game*		m_game = nullptr;
	Vec2        m_position;
	Vec2        m_velocity;
	float       m_orientationDegrees = 0.f;  // counter-clockwise from +x/east
	float		m_turretOrientationDegrees = 0.f;
	float       m_angularVelocity = 0.f;  // (signed) spin rate, in degrees per second, + is counter-clockwise
	float       m_physicsRadius = 5.f;
	float       m_cosmeticRadius = 10.f;
	int         m_health = 1;
	int			m_startingHealth = 1;
	int			m_debrisAmount = 3;
	bool        m_isDead = false;  // gameplay idea
	bool        m_isGarbage = false;  // code idea
	bool		m_noClip = false;
	Vec2		m_desiredMoveDirection = Vec2(0.f,0.f);
	Vec2		m_desiredTurretDirection = Vec2(0.f,0.f);

};