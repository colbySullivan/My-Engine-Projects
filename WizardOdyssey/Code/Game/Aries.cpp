#include "Game/Aries.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

//-----------------------------------------------------------------------------------------------
Aries::Aries( Game* owner, Vec2 const& startPos, float orientationDegrees, EntityFaction faction, Map* map, EntityType type )
	: Entity( owner, startPos, orientationDegrees, faction, map, type )
{
	m_physicsRadius = g_gameConfig->GetValue( "AriesPhysicsRadius", 0.3f );
	m_cosmeticRadius = g_gameConfig->GetValue( "AriesCosmeticRadius", 0.5f );
	m_entityType = type;
	m_isPushedByWalls = true;
	m_isPushedByEntities = false;
	m_doesPushEntities = true;
	m_isHitByBullets = true;
	m_bulletCooldown = 1.3f;
	m_bodyTexture = m_game->m_ariesBodyTexture;
	g_engine->m_render->BindTexture( nullptr );
}

//-----------------------------------------------------------------------------------------------
Aries::~Aries()
{

}

//-----------------------------------------------------------------------------------------------
void Aries::Update( float deltaSeconds )
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
			m_velocity = forwardDir;
			m_position += m_velocity * deltaSeconds * 0.5;
		}
	}
	else
	{
		Wander( deltaSeconds );
	}
}

//-----------------------------------------------------------------------------------------------
void Aries::Render() const
{
	if ( m_isDead )
		return;

	std::vector<Vertex> bodyVerts;
	AddVertsForMe( bodyVerts );
	TransformVertexArrayXY3D( ( int )bodyVerts.size(), bodyVerts.data(), 1.f, m_orientationDegrees, m_position );
	g_engine->m_render->BindTexture( m_bodyTexture );
	g_engine->m_render->DrawVertexArray( ( int )bodyVerts.size(), bodyVerts.data() );

	g_engine->m_render->BindTexture( nullptr );

	if ( m_game->g_drawDebug )
	{
		DebugRender();
	}
}

//-----------------------------------------------------------------------------------------------
bool Aries::TakeDamage( Vec2 bulletPos )
{
	if ( IsPointInsideOrientedSector2D( bulletPos, m_position, m_orientationDegrees, 90.f, m_physicsRadius * 5.f ) )
	{
		//m_game->m_bulletBounce = g_engine->m_audio->StartSound( 5, false, 2.8f );
		return false;
	}
	else
	{
		m_health -= 1;

		if ( m_health <= 0 )
		{
			m_isDead = true;
			return true;
		}
		return true;
	}
}

//-----------------------------------------------------------------------------------------------
void Aries::Respawn()
{

}
