#include "Entity.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"


Entity::Entity(Game* owner, Vec2 const& startPos, float orientationDegrees )
{
	m_game = owner;
	m_position = startPos;
	m_startingHealth = m_health;
	m_orientationDegrees = orientationDegrees;
}

//-----------------------------------------------------------------------------------------------
Entity::~Entity()
{

}

//-----------------------------------------------------------------------------------------------
void Entity::Update( [[maybe_unused]] float deltaSeconds)
{
	if (m_health <= 0 && !m_isDead)
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
}

//-----------------------------------------------------------------------------------------------
void Entity::Die()
{
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

void Entity::InitializeBoxes()
{
	m_turretABB2 = new AABB2( Vec2( 0, 0 ), Vec2( 1, 1 ) );
	m_bodyABB2 = new AABB2( Vec2( 0, 0 ), Vec2( 1, 1 ) );
}

void Entity::AddVertsForMe( std::vector<Vertex>& verts ) const
{
	Vec2 mins( -0.5f, -0.5f );
	Vec2 maxs( 0.5f, 0.5f );
	AABB2 localBox( mins, maxs );
	AddVertsForAABB2D( verts, localBox, Rgba8(255,255,255,255 ));
}

//-----------------------------------------------------------------------------------------------
bool Entity::IsPlayer() const
{
	return false;
}

//-----------------------------------------------------------------------------------------------
EntityType Entity::GetEntityType() const
{
	return m_entityType;  // TODO make neutral
}

//-----------------------------------------------------------------------------------------------
void Entity::Wander( float deltaSeconds )
{
	m_wanderTimer -= deltaSeconds;

	if ( m_wanderTimer <= 0.f )
	{
		m_orientationDegrees = g_rng.RollRandomFloatInRange( 0.f, 360.f );
		m_velocity = Vec2::MakeFromPolarDegrees( m_orientationDegrees, 0.5f );
		m_wanderTimer = 2.0f;
	}

	m_position += m_velocity * deltaSeconds;
}

//-----------------------------------------------------------------------------------------------
void Entity::TryShoot( float deltaSeconds )
{
	m_timeSinceLastShot -= deltaSeconds;
	bool wantsToShoot = g_engine->m_input->IsKeyDown( ' ' ) ||
		g_engine->m_input->GetController( 0 ).GetRightTrigger() > 0.5f;

	if ( wantsToShoot && m_timeSinceLastShot < 0 )
	{
		// Calculate bullet spawn position at turret tip
		float bulletSpawnDist = m_cosmeticRadius; // Or slightly beyond
		Vec2 turretForward = Vec2::MakeFromPolarDegrees( m_turretOrientationDegrees );
		Vec2 bulletSpawnPos = m_position + ( turretForward * bulletSpawnDist );

		m_game->m_currentMap->SpawnNewEntity(
			ENTITY_TYPE_GOOD_BULLET,
			bulletSpawnPos,
			m_turretOrientationDegrees
		);
		m_timeSinceLastShot = SHOOT_COOLDOWN_TIME;
	}
}