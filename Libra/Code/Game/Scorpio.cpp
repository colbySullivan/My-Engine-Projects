#include "Game/Scorpio.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"

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
	m_bodyTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/EnemyTurretBase.png" );
	m_turretTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/EnemyCannon.png" );
	g_engine->m_render->BindTexture( nullptr );
}

//-----------------------------------------------------------------------------------------------
Scorpio::~Scorpio()
{

}

//-----------------------------------------------------------------------------------------------
void Scorpio::Update( float deltaSeconds )
{
	Entity::Update(deltaSeconds);
	m_turretOrientationDegrees += deltaSeconds * 45.f;
	Entity::TryShoot( m_turretOrientationDegrees, deltaSeconds, m_faction );
}

//-----------------------------------------------------------------------------------------------
void Scorpio::Render() const
{
	if ( m_isDead )
		return;

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
void Scorpio::Shoot()
{

}

//-----------------------------------------------------------------------------------------------
void Scorpio::DriveForward( float deltaSeconds )
{

}
