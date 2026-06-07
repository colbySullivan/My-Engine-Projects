#include "Entity.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"


Entity::Entity(Game* owner, Vec2 const& startPos, float orientationDegrees, EntityFaction faction, Map* map, EntityType type )
{
	m_game = owner;
	m_position = startPos;
	m_startingHealth = m_health;
	m_orientationDegrees = orientationDegrees;
	m_faction = faction;
	m_map = map;
	m_entityType = type;
}

//-----------------------------------------------------------------------------------------------
Entity::~Entity()
{

}

//-----------------------------------------------------------------------------------------------
void Entity::Update( [[maybe_unused]] float deltaSeconds)
{
	m_frameTimeEntity += deltaSeconds;
	if (m_health <= 0 || m_isDead)
	{
		Die();
	}
}

//-----------------------------------------------------------------------------------------------
void Entity::Render() const
{

}

//-----------------------------------------------------------------------------------------------
void Entity::DebugRender() const
{
	if ( !m_noClip ) 
	{
		DebugDrawRing(m_position, m_physicsRadius, 0.01f, Rgba8(255,0,255)); // Outer circle
	}
	if ( m_isProtected )
	{
		DebugDrawRing( m_position, m_physicsRadius + 0.1f, 0.1f, Rgba8( 255, 100, 255 ) ); // Outer circle
	}
	DebugDrawRing(m_position, m_cosmeticRadius, 0.01f, Rgba8(0,255,255)); // Inner circle

	Vec2 forwardDebugLine = m_position + GetForwardNormal() * m_cosmeticRadius;
	Vec2 rotatedDebugLine = m_position + GetForwardNormal().GetRotatedBy90Degrees() * m_cosmeticRadius;
	Vec2 velocityDebugLine = m_position + 1.f * m_velocity;
	Vec2 moveTurretDirEnd = m_cosmeticRadius * 1.0f * Vec2::MakeFromPolarDegrees(m_desiredTurretDirection.GetOrientationDegrees());
	Vec2 moveDirEnd = m_cosmeticRadius * 1.0f * Vec2::MakeFromPolarDegrees(m_desiredMoveDirection.GetOrientationDegrees());

	DebugDrawLine( m_position + 1.0f * moveDirEnd, m_position + 1.1f * moveDirEnd, 0.1f, Rgba8( 0, 255, 255 ) ); // Tank direction
	DebugDrawLine( m_position + 1.0f * moveTurretDirEnd, m_position + 1.1f * moveTurretDirEnd, 0.1f, Rgba8( 255, 0, 255 ) ); // Turret
	DebugDrawLine(m_position, forwardDebugLine, 0.03f, Rgba8(255, 0, 0)); // Forward line
	DebugDrawLine(m_position, rotatedDebugLine, 0.03f, Rgba8(0, 255, 0)); // Right line
	DebugDrawLine(m_position, velocityDebugLine, 0.01f, Rgba8(255,255,0)); // Velocity line
	if ( m_targetPos != Vec2( 0.f, 0.f ) )
	{
		DebugDrawLine( m_position, m_targetPos, 0.01f, Rgba8( 255, 255, 0 ) ); // Point goal line
	}
}

//-----------------------------------------------------------------------------------------------
void Entity::Die()
{
	if ( !m_justDied )
	{
		m_frameTimeEntity = 0.0f;
		m_justDied = true;
	}
	PlayDeathExplosion();
	//m_game->m_enemyDied = g_engine->m_audio->StartSound( 4, false, 0.8f );
	m_isGarbage = true;
	m_isDead = true;
}

//-----------------------------------------------------------------------------------------------
bool Entity::IsOffscreen() const
{
	return (m_position.x > WORLD_SIZE_X + m_cosmeticRadius || m_position.y > WORLD_SIZE_Y + m_cosmeticRadius || m_position.x < 0 - m_cosmeticRadius || m_position.y < 0 - m_cosmeticRadius);
}

//-----------------------------------------------------------------------------------------------
Vec2 Entity::GetForwardNormal() const
{
	return Vec2(CosDegrees(m_orientationDegrees), SinDegrees(m_orientationDegrees));
}

//-----------------------------------------------------------------------------------------------
void Entity::WrapAroundScreen()
{
	// Wrap horizontally
	if ( m_position.x > WORLD_SIZE_X + m_cosmeticRadius )
	{
		m_position.x = -m_cosmeticRadius;
	}
	else if ( m_position.x < -m_cosmeticRadius )
	{
		m_position.x = WORLD_SIZE_X + m_cosmeticRadius;
	}

	// Wrap vertically
	if ( m_position.y > WORLD_SIZE_Y + m_cosmeticRadius )
	{
		m_position.y = -m_cosmeticRadius;
	}
	else if ( m_position.y < -m_cosmeticRadius )
	{
		m_position.y = WORLD_SIZE_Y + m_cosmeticRadius;
	}
}

//-----------------------------------------------------------------------------------------------
void Entity::spawnRandomEdge()
{
	int spawnEdge = g_rng.RollRandomIntInRange( 0, 3 );

	switch ( spawnEdge )
	{
	case 0: // Left
		m_position.x = -m_cosmeticRadius;
		m_position.y = g_rng.RollRandomFloatInRange( 0, WORLD_SIZE_Y );
		break;

	case 1: // Right
		m_position.x = WORLD_SIZE_X + m_cosmeticRadius;
		m_position.y = g_rng.RollRandomFloatInRange( 0, WORLD_SIZE_Y );
		break;

	case 2: // Top
		m_position.x = g_rng.RollRandomFloatInRange( 0, WORLD_SIZE_X );
		m_position.y = WORLD_SIZE_Y + m_cosmeticRadius;
		break;

	case 3: // Bottom
		m_position.x = g_rng.RollRandomFloatInRange( 0, WORLD_SIZE_X );
		m_position.y = -m_cosmeticRadius;
		break;
	}
}

//-----------------------------------------------------------------------------------------------
void Entity::SearchForPlayerAndTryToShoot( float deltaSeconds )
{
	m_targetPos = Vec2( 0.f, 0.f );
	Entity* player = m_map->m_entityListsByType[ENTITY_TYPE_GOOD_PLAYER][0];
	if ( m_map->HasLineOfSight( player->m_position, m_position ) && m_map->IsPlayerAlive())
	{
		m_targetPos = player->m_position;
		Vec2 toPlayerPos = player->m_position - m_position;
		Vec2 directionToPlayer = toPlayerPos.GetNormalized();
		m_orientationDegrees = GetTurnedTowardDegrees( m_orientationDegrees, Atan2Degrees( directionToPlayer.y, directionToPlayer.x ), .5f );
	}
	if ( m_targetPos != Vec2( 0.f, 0.f ) && m_position != m_targetPos)
	{
		//TurnedTowardPosition( player->m_position, deltaSeconds );

		if ( IsPointInsideOrientedSector2D( m_targetPos, m_position, m_orientationDegrees, 90.f, LEO_MAX_VIS ) )
		{
			//DriveForward( deltaSeconds );
			Vec2 forwardDir = GetForwardNormal();
			m_velocity = forwardDir;
			m_position += m_velocity * deltaSeconds * 0.5;

			if ( IsPointInsideOrientedSector2D( m_targetPos, m_position, m_orientationDegrees, 5.f, LEO_MAX_VIS ) )
			{
				TryShoot( m_orientationDegrees, deltaSeconds, m_faction );
			}
		}
	}
	else
	{
		Wander( deltaSeconds );
	}
}

//-----------------------------------------------------------------------------------------------
void Entity::InitializeBoxes()
{
	m_turretABB2 = new AABB2( Vec2( 0, 0 ), Vec2( 1, 1 ) );
	m_bodyABB2 = new AABB2( Vec2( 0, 0 ), Vec2( 1, 1 ) );
}

//-----------------------------------------------------------------------------------------------
void Entity::AddVertsForMe( std::vector<Vertex>& verts ) const
{
	Vec2 mins( -0.5f, -0.5f );
	Vec2 maxs( 0.5f, 0.5f );
	AABB2 localBox( mins, maxs );
	AddVertsForAABB2D( verts, localBox, Rgba8(255,255,255,255 ));
}

//------------------------------------------------------------------------------
void Entity::PlayDeathExplosion() const
{
	std::vector<Vertex> explosionVerts;
	std::vector<Vertex> tileVerts;

	const SpriteDefinition& explosionSprite = g_game->m_tilesSpriteSheetAnim->GetSpriteDefAtTime(m_frameTimeEntity);
	Vec2 explosionMins, explosionMaxs;
	explosionSprite.GetUVs(explosionMins, explosionMaxs);

	Vec2 mins(m_position.x - 0.5f, m_position.y - 0.5f);
	Vec2 maxs(m_position.x + 0.5f, m_position.y + 0.5f);
	AABB2 localBox(mins, maxs);

	AddVertsForAABB2D(explosionVerts, localBox, Rgba8(255, 255, 255), explosionMins, explosionMaxs);

	g_engine->m_render->BindTexture(&g_game->m_explosionSpriteSheet->GetTexture());
	g_engine->m_render->DrawVertexArray(explosionVerts);
 	g_engine->m_render->BindTexture(nullptr);
}

//-----------------------------------------------------------------------------------------------
bool Entity::IsPlayer() const
{
	return false;
}

//-----------------------------------------------------------------------------------------------
void Entity::Respawn()
{

}

//-----------------------------------------------------------------------------------------------
EntityType Entity::GetEntityType() const
{
	return m_entityType;
}

//-----------------------------------------------------------------------------------------------
void Entity::Wander( float deltaSeconds )
{
	m_wanderTimer -= deltaSeconds;
	if ( m_wanderTimer <= 0.f )
	{
		m_goalOrientationDegrees = g_rng.RollRandomFloatInRange( 0.f, 360.f );
		m_wanderTimer = m_wanderDuration;
	}
	float turnSpeed = 180.f;
	float maxTurnThisFrame = turnSpeed * deltaSeconds;
	m_orientationDegrees = GetTurnedTowardDegrees(m_orientationDegrees, m_goalOrientationDegrees, maxTurnThisFrame);
	Vec2 forwardDir = GetForwardNormal();
	m_velocity = forwardDir;
	m_position += m_velocity * deltaSeconds * 0.5;
}

//-----------------------------------------------------------------------------------------------
void Entity::TryShoot( float fireOrientation, float deltaSeconds, EntityFaction faction )
{
	m_timeSinceLastShot -= deltaSeconds;

	if ( m_timeSinceLastShot < 0 )
	{
		float bulletSpawnDist = m_cosmeticRadius;
		Vec2 turretForward = Vec2::MakeFromPolarDegrees( fireOrientation );
		Vec2 bulletSpawnPos = m_position + ( turretForward * bulletSpawnDist );

		//m_game->m_shootSound = g_engine->m_audio->StartSound( 3, false, 0.8f );
		m_map->SpawnNewEntity( ENTITY_TYPE_GOOD_BULLET, bulletSpawnPos, fireOrientation, faction );
		m_timeSinceLastShot = m_bulletCooldown;
	}
}

//-----------------------------------------------------------------------------------------------
bool Entity::TakeDamage( [[maybe_unused]] Vec2 bulletPos )
{
	if ( m_health <= 0 )
	{
		m_isDead = true;
	}
	if ( !m_isProtected )
	{
		m_health -= 1;
	}
	//m_game->m_playerHit = g_engine->m_audio->StartSound( 7, false, 0.8f );
	return true;
}

