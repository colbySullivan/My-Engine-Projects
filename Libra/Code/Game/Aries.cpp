#include "Game/Aries.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"

//-----------------------------------------------------------------------------------------------
Aries::Aries( Game* owner, Vec2 const& startPos )
	: Entity( owner, startPos )
{
	m_physicsRadius = PLAYER_PHYSICS_RADIUS;
	m_cosmeticRadius = PLAYER_COSMETIC_RADIUS;
	m_isPushedByWalls = true;
	m_isPushedByEntities = false;
	m_doesPushEntities = true;
	m_isHitByBullets = true;
	m_bodyTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/EnemyAries.png" );
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

	m_velocity = Vec2( 1.f, 0.f );

	m_turretOrientationDegrees += deltaSeconds * 45.f;

	m_position += m_velocity * deltaSeconds;
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
void Aries::Shoot()
{

}

//-----------------------------------------------------------------------------------------------
void Aries::DriveForward( float deltaSeconds )
{

}
