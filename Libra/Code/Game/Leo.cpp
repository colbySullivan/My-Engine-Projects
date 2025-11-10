#include "Game/Leo.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Game/Map.hpp"

//-----------------------------------------------------------------------------------------------
Leo::Leo( Game* owner, Vec2 const& startPos, float orientationDegrees, EntityFaction faction, Map* map, EntityType type )
	: Entity( owner, startPos, orientationDegrees, faction, map, type )
{
	m_physicsRadius = 0.4f;
	m_cosmeticRadius = 0.5f;
	m_isPushedByWalls = true;
	m_isPushedByEntities = true;
	m_doesPushEntities = true;
	m_isHitByBullets = true;
	m_wanderTimer = 2.0f;
	m_bodyTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/EnemyTank4.png" );
	g_engine->m_render->BindTexture( nullptr );
}

//-----------------------------------------------------------------------------------------------
Leo::~Leo()
{
	
}


//-----------------------------------------------------------------------------------------------
void Leo::Update( float deltaSeconds )
{
	Entity::Update(deltaSeconds);
	Entity::SearchForPlayerAndTryToShoot(deltaSeconds);
}


void Leo::Render() const
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
void Leo::DriveForward( float deltaSeconds )
{

}


