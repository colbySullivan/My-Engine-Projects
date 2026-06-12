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
	m_spawnTimer = new Timer( 1.f );
	m_spawnTimer->Start();
	m_spawnTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/x_mark.png" );
}

//-----------------------------------------------------------------------------------------------
Enemy::~Enemy()
{

}

//-----------------------------------------------------------------------------------------------
void Enemy::Update( float deltaSeconds )
{
	if ( m_spawnTimer->DecrementPeriodIfElapsed() )
	{
		m_spawnTimer->Stop();
	}
	if ( m_spawnTimer->IsStopped() )
	{
		Entity::Update( deltaSeconds );
		UpdateMoveIfSpawned();
	}
	
}

//-----------------------------------------------------------------------------------------------
void Enemy::Render() const
{
	if ( m_isDead )
		return;

	if ( m_game->g_drawDebug )
	{
		DebugRender();
	}

	if ( !m_spawnTimer->IsStopped() )
	{
		RenderSpawnX();
		return;
	}
	Entity::Render();
}

//-----------------------------------------------------------------------------------------------
bool Enemy::TakeDamage()
{
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

void Enemy::UpdateMoveIfSpawned()
{
	m_targetPos = Vec2( 0.f, 0.f );
	Entity* player = m_map->m_entityListsByType[ENTITY_TYPE_GOOD_PLAYER][0];

	float deltaSeconds = g_engine->m_systemClock->GetDeltaSeconds();

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
void Enemy::RenderSpawnX() const
{
	float halfWidth = 0.5f;
	float halfHeight = 0.5f;

	Vec2 mins( m_position.x - halfWidth, m_position.y - halfHeight );
	Vec2 maxs( m_position.x + halfWidth, m_position.y + halfHeight );
	AABB2 localBox( mins, maxs );

	std::vector<Vertex> spriteVerts;
	AddVertsForAABB2D( spriteVerts, localBox, Rgba8( 255, 255, 255 ) );
	g_engine->m_render->BindTexture( m_spawnTexture );
	g_engine->m_render->DrawVertexArray( spriteVerts );
	g_engine->m_render->BindTexture( nullptr );
}
