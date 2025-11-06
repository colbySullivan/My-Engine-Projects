#include "Game/Bullet.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Game/Map.hpp"

//-----------------------------------------------------------------------------------------------
Bullet::Bullet( Game* owner, Vec2 const& startPos, float orientationDegrees )
	: Entity( owner, startPos, orientationDegrees )
{
	m_physicsRadius = BULLET_PHYSICS_RADIUS;
	m_cosmeticRadius = BULLET_COSMETIC_RADIUS;
	m_isPushedByWalls = false;
	m_isPushedByEntities = false;
	m_doesPushEntities = false;
	m_isHitByBullets = false;
	m_velocity = Vec2::MakeFromPolarDegrees( orientationDegrees, BULLET_SPEED );
	m_bodyTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/FriendlyShell.png" );
	m_entityType = ENTITY_TYPE_GOOD_BULLET;
	g_engine->m_render->BindTexture( nullptr );
	m_bulletLifeTime = BULLET_LIFETIME_SECONDS;
}

//-----------------------------------------------------------------------------------------------
Bullet::~Bullet()
{

}


//-----------------------------------------------------------------------------------------------
void Bullet::Update( float deltaSeconds )
{
	if ( m_bulletLifeTime < 0 )
	{
		m_isDead = true;
		return;
	}

	Entity::Update( deltaSeconds );
	m_bulletLifeTime -= deltaSeconds;

	Vec2 nextPosition = m_position + m_velocity * deltaSeconds;
	IntVec2 nextTileCoords = m_game->m_currentMap->GetTileCoordsForWorldPos( nextPosition );
	IntVec2 currentTileCoords = m_game->m_currentMap->GetTileCoordsForWorldPos( m_position );

	if ( m_game->m_currentMap->IsTileSolidAtTileCoords( nextTileCoords ) )
	{
		Vec2 surfaceNormal = Vec2( static_cast<float>( currentTileCoords.x - nextTileCoords.x ), static_cast<float>( currentTileCoords.y - nextTileCoords.y ) );
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

	CheckEntityCollisions();
}


void Bullet::Render() const
{
	if ( m_isDead )
		return;

	std::vector<Vertex> bodyVerts;
	AddVertsForMe( bodyVerts );
	TransformVertexArrayXY3D( ( int )bodyVerts.size(), bodyVerts.data(), .1f, m_orientationDegrees, m_position );
	g_engine->m_render->BindTexture( m_bodyTexture );
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
void Bullet::CheckEntityCollisions()
{
	EntityList const& entities = m_game->m_currentMap->m_allEntities;
	for ( int i = 0; i < entities.size(); i++ )
	{
		Entity* other = entities[i];
		if ( !other || other->m_isDead || other == this ) // TODO skip if bullet
			continue;
		if ( other->m_faction != m_faction )
		{
			if ( DoDiscsOverlap( m_position, m_physicsRadius, other->m_position, other->m_physicsRadius ) )
			{
				// TODO Deal damage
				m_isDead = true;
				return;
			}
		}
	}
}
