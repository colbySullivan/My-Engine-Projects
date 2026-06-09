#include "Game/Bullet.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/Map.hpp"

//-----------------------------------------------------------------------------------------------
Bullet::Bullet( Game* owner, Vec2 const& startPos, float orientationDegrees, EntityFaction faction, Map* map, EntityType type )
	: Entity( owner, startPos, orientationDegrees, faction, map, type )
{
	m_physicsRadius = g_gameConfig->GetValue("bulletPhysicsRadius", 0.1f);
	m_cosmeticRadius = g_gameConfig->GetValue("bulletCosmeticRadius", 0.15f);
	m_isPushedByWalls = false;
	m_isPushedByEntities = false;
	m_doesPushEntities = false;
	m_isHitByBullets = false;
	if ( faction == FACTION_EVIL )
	{
		m_gunTexture = m_game->m_badBulletTexture;
	}
	else
	{
		m_gunTexture = m_game->m_goodBulletTexture;
	}
	m_entityType = ENTITY_TYPE_GOOD_BULLET;
	g_engine->m_render->BindTexture( nullptr );
}

//-----------------------------------------------------------------------------------------------
Bullet::~Bullet()
{

}


//-----------------------------------------------------------------------------------------------
void Bullet::Update( float deltaSeconds )
{
	if ( m_isDead )
	{
		m_isDead = true;
		Die();
		return;
	}

	Entity::Update( deltaSeconds );
	SpawnAndShoot( deltaSeconds );
	CheckEntityCollisions();
}


void Bullet::Render() const
{
	if ( m_isDead )
		return;

	std::vector<Vertex> bodyVerts;
	AddVertsForMe( bodyVerts );
	TransformVertexArrayXY3D( ( int )bodyVerts.size(), bodyVerts.data(), .1f, m_orientationDegrees, m_position );
	g_engine->m_render->BindTexture( m_gunTexture );
	g_engine->m_render->DrawVertexArray( ( int )bodyVerts.size(), bodyVerts.data() );

	g_engine->m_render->BindTexture( nullptr );

	if ( m_game->g_drawDebug )
	{
		DebugRender();
	}
}

//-----------------------------------------------------------------------------------------------
void Bullet::AddVertsForMe( std::vector<Vertex>& verts ) const
{
	Vec2 mins( -1.f, -0.5f );
	Vec2 maxs( 2.f, 0.5f );
	AABB2 localBox( mins, maxs );
	AddVertsForAABB2D( verts, localBox, Rgba8( 255, 255, 255, 255 ) );
}

//-----------------------------------------------------------------------------------------------
void Bullet::Die()
{
	m_isGarbage = true;
	m_isDead = true;
}

//-----------------------------------------------------------------------------------------------
void Bullet::Respawn()
{

}

//-----------------------------------------------------------------------------------------------
void Bullet::CheckEntityCollisions()
{
	EntityList const& entities = m_game->m_currentMap->m_allEntities;
	for ( int i = 0; i < static_cast<int>(entities.size()); i++ )
	{
		Entity* other = entities[i];
		if ( !other || other->m_isDead || other == this )
			continue;
		if ( other->m_faction != m_faction )
		{
			if ( DoDiscsOverlap( m_position, m_physicsRadius, other->m_position, other->m_physicsRadius ) )
			{
				if ( other->TakeDamage( m_position ) )
				{
					m_isDead = true;
				}
				else
				{
					Vec2 reflectNormal = ( m_position - other->m_position ).GetNormalized();
					m_velocity = m_velocity.GetReflected( reflectNormal );
					m_orientationDegrees = m_velocity.GetOrientationDegrees();
				}
				return;
			}
		}
	}
}

void Bullet::SpawnAndShoot( float deltaSeconds )
{
	m_velocity = Vec2::MakeFromPolarDegrees( m_orientationDegrees, BULLET_SPEED );
	Vec2 nextPosition = m_position + m_velocity * deltaSeconds;
	IntVec2 nextTileCoords = m_game->m_currentMap->GetTileCoordsForWorldPos( nextPosition );
	IntVec2 currentTileCoords = m_game->m_currentMap->GetTileCoordsForWorldPos( m_position );

	if ( m_game->m_currentMap->IsTileSolidAtTileCoords( nextTileCoords ) )
	{
		Vec2 surfaceNormal = Vec2( static_cast< float >( currentTileCoords.x - nextTileCoords.x ), static_cast< float >( currentTileCoords.y - nextTileCoords.y ) );
		surfaceNormal.Normalize();

		m_velocity = m_velocity.GetReflected( surfaceNormal );
		m_orientationDegrees = m_velocity.GetOrientationDegrees();

		if ( m_entityType == ENTITY_TYPE_GOOD_BULLET )
		{
			m_bouncesRemaining--;
			if ( m_bouncesRemaining < 0 )
			{
				m_isDead = true;
				return;
			}
		}
	}
	else
	{
		m_position = nextPosition;
	}
}
