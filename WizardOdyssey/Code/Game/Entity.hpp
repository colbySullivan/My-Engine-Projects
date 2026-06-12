#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Vertex.hpp"
#include <vector>
#include "Game/SpriteAnimationDefinition.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "ActorDefinitions.hpp"


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
	ENTITY_TYPE_GOOD_PLAYER = 0,

	ENTITY_TYPE_EVIL_SMALL_BALD_DUDE = 1,
	ENTITY_TYPE_EVIL_SMALL_LASER_DUDE = 2,
	ENTITY_TYPE_EVIL_SMALL_BONE_DUDE = 3,
	ENTITY_TYPE_EVIL_SMALL_PISTACHIO_DUDE = 4,
	ENTITY_TYPE_EVIL_SMALL_GOBLIN_DUDE = 5,
	ENTITY_TYPE_EVIL_SMALL_FISH_DUDE = 6,
	ENTITY_TYPE_EVIL_MEDIUM_GOBLIN_DUDE = 7,

	ENTITY_TYPE_EVIL_BIG_CHIEF_DUDE = 8,
	ENTITY_TYPE_EVIL_BIG_KNIFE_DUDE = 9,
	ENTITY_TYPE_EVIL_BIG_MAMA_DUDE = 10,
	ENTITY_TYPE_EVIL_BIG_DUDE_EATING = 11,
	ENTITY_TYPE_EVIL_BIG_ARMOURED_GOBLIN_DUDE = 12,
	ENTITY_TYPE_EVIL_BIG_ARM_DUDE = 13,

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
	Entity(Game* owner, Vec2 const& startPos, float orientationDegrees, EntityFaction faction, Map* map, EntityType type, std::string defName="SmallBaldDude");
	virtual ~Entity();

	virtual void Update(float deltaSeconds) = 0;
	virtual void Render() const = 0;
	virtual void DebugRender() const;
	virtual void Die();
	virtual bool IsPlayer() const;
	virtual void Respawn();
	virtual EntityType GetEntityType() const;
	virtual void TryShoot( float fireOrientation, float deltaSeconds, EntityFaction faction );
	virtual bool TakeDamage();

	void		Wander( float deltaSeconds );
	bool        IsOffscreen() const;
	Vec2        GetForwardNormal() const;
	void		WrapAroundScreen();
	void		spawnRandomEdge();
	void		SearchForPlayerAndTryToShoot( float deltaSeconds );

	void InitializeBoxes();
	void InitializeSpriteSheet();
	void InitializeDefitionStats();
	void AddVertsForMe( std::vector<Vertex>& verts ) const;
	void PlayDeathExplosion() const;

	RandomNumberGenerator g_rng;

public:
	Game* m_game = nullptr;
	Vec2			m_position;
	Vec2			m_velocity;
	Vec2			m_scale;
	float			m_walkSpeed;
	float			m_orientationDegrees = 0.f;  // counter-clockwise from +x/east
	float			m_turretOrientationDegrees = 0.f;
	float			m_angularVelocity = 0.f;  // (signed) spin rate, in degrees per second, + is counter-clockwise
	float			m_physicsRadius;
	float			m_cosmeticRadius;
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
	const ActorDefinitions* m_actorDef = nullptr;
	// Animations
	const SpriteAnimationDefinition* m_spriteAnimationDef = nullptr;
	const SpriteAnimationGroupDefinition* m_currentAnimGroup = nullptr;
	double m_currentAnimStartTime = 0.0;
	SpriteSheet* m_currentSpriteSheet = nullptr;
	Timer* m_animTimer = nullptr;
	std::string		m_defName = "Wizard";

	//TileHeatMap*	m_heatMap = nullptr; // [[MAYBE]] TODO personal heat map used for pathing
};