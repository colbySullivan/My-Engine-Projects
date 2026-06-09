#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Vertex.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------

class RandomNumberGenerator;
class Game;
class Texture;
class Entity;
struct AABB2;
class Map;

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
enum EntityFaction
{
	FACTION_GOOD,
	FACTION_NEUTRAL,
	FACTION_EVIL
};

//-----------------------------------------------------------------------------------------------
typedef std::vector<Entity*> EntityList;

//-----------------------------------------------------------------------------------------------
class Entity
{
public:
	Entity(Game* owner, Vec2 const& startPos, float orientationDegrees, EntityFaction faction, Map* map, EntityType type);
	virtual ~Entity();

	virtual void Update(float deltaSeconds) = 0;
	virtual void Render() const = 0;
	virtual void DebugRender() const;
	virtual void Die();
	virtual bool IsPlayer() const;
	virtual void Respawn();
	virtual EntityType GetEntityType() const;
	virtual void TryShoot( float fireOrientation, float deltaSeconds, EntityFaction faction );
	virtual bool TakeDamage( Vec2 bulletPos );

	void		Wander( float deltaSeconds );
	bool        IsOffscreen() const;
	Vec2        GetForwardNormal() const;
	void		WrapAroundScreen();
	void		spawnRandomEdge();
	void		SearchForPlayerAndTryToShoot( float deltaSeconds );

	void InitializeBoxes();
	void AddVertsForMe( std::vector<Vertex>& verts ) const;
	void PlayDeathExplosion() const;

	RandomNumberGenerator g_rng;

public:
	Game* m_game = nullptr;
	Vec2			m_position;
	Vec2			m_velocity;
	float			m_orientationDegrees = 0.f;  // counter-clockwise from +x/east
	float			m_turretOrientationDegrees = 0.f;
	float			m_angularVelocity = 0.f;  // (signed) spin rate, in degrees per second, + is counter-clockwise
	float			m_physicsRadius = 5.f;
	float			m_cosmeticRadius = 10.f;
	int				m_health = 3;
	int				m_startingHealth = 1;
	int				m_debrisAmount = 3;
	bool			m_isDead = false;  // gameplay idea
	bool			m_isGarbage = false;  // code idea
	bool			m_noClip = false;
	Vec2			m_desiredMoveDirection = Vec2( 0.f, 0.f );
	Vec2			m_desiredTurretDirection = Vec2( 0.f, 0.f );
	Texture*		m_gunTexture = nullptr;
	Texture*		m_wizardTexture = nullptr;
	AABB2*			m_turretABB2 = nullptr;
	AABB2*			m_bodyABB2 = nullptr;
	EntityFaction	m_faction = FACTION_NEUTRAL;
	bool			m_isPushedByEntities = false;
	bool			m_doesPushEntities = false;
	bool			m_isPushedByWalls = false;
	bool			m_isHitByBullets = false;
	float			m_wanderTimer = 0.f;
	float			m_wanderDuration = 2.f;
	EntityType		m_entityType = NUM_ENTITY_TYPES;
	float			m_goalOrientationDegrees;
	Map*			m_map = nullptr;
	Vec2			m_targetPos;
	float			m_timeSinceLastShot;
	float			m_bulletCooldown;
	bool			m_isProtected = false;
	float			m_frameTimeEntity = 0.f;
	bool			m_justDied = false;
	//TileHeatMap*	m_heatMap = nullptr; // [[MAYBE]] TODO personal heat map used for pathing

};