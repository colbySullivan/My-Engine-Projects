#include "Game/Enemy.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

//-----------------------------------------------------------------------------------------------
Enemy::Enemy( Game* owner, Vec2 const& startPos, float orientationDegrees, EntityFaction faction, Map* map, EntityType type, std::string defName )
	: Entity( owner, startPos, orientationDegrees, faction, map, type )
{
	m_entityType = type;
	m_defName = defName;
	InitializeDefitionStats();
	m_bulletCooldown = 1.3f;
	m_gunTexture = m_game->m_ariesBodyTexture;
	InitializeSpriteSheet();
}

//-----------------------------------------------------------------------------------------------
Enemy::~Enemy()
{

}

//-----------------------------------------------------------------------------------------------
void Enemy::Update( float deltaSeconds )
{
	Entity::Update( deltaSeconds );
	m_targetPos = Vec2( 0.f, 0.f );
	Entity* player = m_map->m_entityListsByType[ENTITY_TYPE_GOOD_PLAYER][0];
	if ( m_map->HasLineOfSight( player->m_position, m_position ) && m_map->IsPlayerAlive() )
	{
		m_targetPos = player->m_position;
		Vec2 toPlayerPos = player->m_position - m_position;
		Vec2 directionToPlayer = toPlayerPos.GetNormalized();
		m_orientationDegrees = GetTurnedTowardDegrees( m_orientationDegrees, Atan2Degrees( directionToPlayer.y, directionToPlayer.x ), .5f );
	}
	if ( m_targetPos != Vec2( 0.f, 0.f ) && m_position != m_targetPos )
	{
		if ( IsPointInsideOrientedSector2D( m_targetPos, m_position, m_orientationDegrees, 90.f, LEO_MAX_VIS ) )
		{
			Vec2 forwardDir = GetForwardNormal();
			m_velocity = forwardDir * m_walkSpeed;
			m_position += m_velocity * deltaSeconds * 0.5;
		}
	}
	else
	{
		Wander( deltaSeconds );
	}
}

//-----------------------------------------------------------------------------------------------
void Enemy::Render() const
{
	if ( m_isDead )
		return;

	Entity::Render();

	if ( m_game->g_drawDebug )
	{
		DebugRender();
	}
}

//-----------------------------------------------------------------------------------------------
bool Enemy::TakeDamage()
{
	//if ( IsPointInsideOrientedSector2D( bulletPos, m_position, m_orientationDegrees, 90.f, m_physicsRadius * 5.f ) )
	//{
	//	//m_game->m_bulletBounce = g_engine->m_audio->StartSound( 5, false, 2.8f );
	//	return false;
	//}
	//else
	//{
	//	m_health -= 1;

	//	if ( m_health <= 0 )
	//	{
	//		m_isDead = true;
	//		return true;
	//	}
	//	return true;
	//}

	m_health -= 1;

	if ( m_health <= 0 )
	{
		m_isDead = true;
		return true;
	}
	return true;
}

//-----------------------------------------------------------------------------------------------
void Enemy::Respawn()
{

}
