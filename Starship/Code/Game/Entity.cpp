#include "Entity.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Engine.hpp"
#include "Game/App.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/Debris.hpp"
#include "Game/GameCommon.hpp"

Entity::Entity(Game* owner, Vec2 const& startPos )
{
	m_game = owner;
	m_position = startPos;
	m_startingHealth = m_health;
	//m_velocity = velocity;
	//m_health = health;
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
	DebugDrawRing(m_position, m_physicsRadius, 0.2f, Rgba8(255,0,255)); // Outer circle
	DebugDrawRing(m_position, m_cosmeticRadius, 0.2f, Rgba8(0,255,255)); // Inner circle

	Vec2 playerShipPos = g_theApp->m_game->m_playerShip->m_position;
	Vec2 forwardDebugLine = m_position + GetForwardNormal() * m_cosmeticRadius;
	Vec2 rotatedDebugLine = m_position + GetForwardNormal().GetRotatedBy90Degrees() * m_cosmeticRadius;
	Vec2 velocityDebugLine = m_position + 1.f * m_velocity;

	DebugDrawLine(playerShipPos, m_position, 0.2f, Rgba8(50, 50, 50)); // Line to entities
	DebugDrawLine(m_position, forwardDebugLine, 0.2f, Rgba8(255, 0, 0)); // Forward line
	DebugDrawLine(m_position, rotatedDebugLine, 0.2f, Rgba8(0, 255, 0)); // Right line
	DebugDrawLine(m_position, velocityDebugLine, 0.2f, Rgba8(255,255,0)); // Velocity line


}

//-----------------------------------------------------------------------------------------------
void Entity::Die()
{
	/*SoundPlaybackID temp = g_engine->m_audio->StartSound( 4, false, 0.3f);
	m_game->HandleSound( temp, PRIORITY_HIGH, 0.3f );*/
	m_isGarbage = true;
	m_isDead = true;
	g_theApp->m_game->m_enemiesKilled += 1;
	g_theApp->m_game->SpawnDebrisCluster(m_position, m_startingEntityColor, m_velocity, m_debrisAmount, m_debrisSize);
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

//------------------------------------------------------------------------------
void Entity::HealthVisual()
{
	m_entityColor.r =  static_cast<unsigned char>(Interpolate(static_cast<unsigned char>(m_startingEntityColor.r), 255.f, static_cast<float>(m_health) / static_cast<float>(m_startingHealth)));
	m_entityColor.g =  static_cast<unsigned char>(Interpolate(static_cast<unsigned char>(m_startingEntityColor.g), 255.f, static_cast<float>(m_health) / static_cast<float>(m_startingHealth)));
	m_entityColor.b =  static_cast<unsigned char>(Interpolate(static_cast<unsigned char>(m_startingEntityColor.b), 255.f, static_cast<float>(m_health) / static_cast<float>(m_startingHealth)));
}

