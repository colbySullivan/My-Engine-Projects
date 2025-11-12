#include "Game/Scorpio.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Game/Map.hpp"

//-----------------------------------------------------------------------------------------------
Scorpio::Scorpio( Game* owner, Vec2 const& startPos, float orientationDegrees, EntityFaction faction, Map* map, EntityType type )
	: Entity( owner, startPos, orientationDegrees, faction, map, type )
{
	m_physicsRadius = PLAYER_PHYSICS_RADIUS;
	m_cosmeticRadius = PLAYER_COSMETIC_RADIUS;
	m_isPushedByWalls = true;
	m_isPushedByEntities = false;
	m_doesPushEntities = true;
	m_isHitByBullets = true;
	m_bulletCooldown = 0.3f;
	m_bodyTexture = m_game->m_scorpioBodyTexture;
	m_turretTexture = m_game->m_scorpioTurretTexture;
	g_engine->m_render->BindTexture( nullptr );
}

//-----------------------------------------------------------------------------------------------
Scorpio::~Scorpio()
{

}

//-----------------------------------------------------------------------------------------------
void Scorpio::Update( float deltaSeconds )
{
	Entity::Update( deltaSeconds );
	if ( m_map->m_entityListsByType[ENTITY_TYPE_GOOD_PLAYER].empty() )
	{
		m_turretOrientationDegrees += deltaSeconds * 45.f;
		return;
	}

	Entity* player = m_map->m_entityListsByType[ENTITY_TYPE_GOOD_PLAYER][0];
	if ( player && !player->m_isDead && m_map->HasLineOfSight( m_position, player->m_position ) )
	{
		Vec2 toPlayerPos = player->m_position - m_position;
		Vec2 directionToPlayer = toPlayerPos.GetNormalized();
		float targetAngle = Atan2Degrees( directionToPlayer.y, directionToPlayer.x );

		m_turretOrientationDegrees = GetTurnedTowardDegrees( m_turretOrientationDegrees, targetAngle, 20.f * deltaSeconds );

		if ( IsPointInsideOrientedSector2D( player->m_position, m_position, m_turretOrientationDegrees, 10.f, LEO_MAX_VIS ) )
		{
			TryShoot( m_turretOrientationDegrees, deltaSeconds, m_faction );
		}
	}
	else
	{
		m_turretOrientationDegrees += deltaSeconds * 45.f;
	}
}

//-----------------------------------------------------------------------------------------------
void Scorpio::Render() const
{
	if ( m_isDead )
		return;

	std::vector<Vertex> lineOfSightLaser;
	Vec2 turretForward = Vec2( CosDegrees( m_turretOrientationDegrees ), SinDegrees( m_turretOrientationDegrees ) );
	RaycastResult2D laserResult = m_map->RaycastVsTiles( m_position, turretForward, SCORPIO_MAX_VIS );
	AddVertsForLineSegment2D( lineOfSightLaser, m_position, laserResult.m_impactPos, Vec2( 0.03f, 0.03f ), Rgba8( 255, 0, 0 ) );
	g_engine->m_render->DrawVertexArray( ( int )lineOfSightLaser.size(), lineOfSightLaser.data() );

	std::vector<Vertex> bodyVerts;
	AddVertsForMe(bodyVerts);
	TransformVertexArrayXY3D( (int)bodyVerts.size(), bodyVerts.data(), 1.f, m_orientationDegrees, m_position );
	g_engine->m_render->BindTexture( m_bodyTexture );
	g_engine->m_render->DrawVertexArray( (int)bodyVerts.size(), bodyVerts.data() );

	std::vector<Vertex> turretVerts;
	AddVertsForMe(turretVerts);
	TransformVertexArrayXY3D( (int)turretVerts.size(), turretVerts.data(), 1.f, m_turretOrientationDegrees, m_position );
	g_engine->m_render->BindTexture( m_turretTexture );
	g_engine->m_render->DrawVertexArray( (int)turretVerts.size(), turretVerts.data() );

	g_engine->m_render->BindTexture( nullptr );

	if ( m_game->g_drawDebug )
	{
		DebugRender();
	}
}

//-----------------------------------------------------------------------------------------------
void Scorpio::Respawn()
{

}
