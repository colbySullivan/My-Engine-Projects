#include "Game/Leo.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"

//-----------------------------------------------------------------------------------------------
Leo::Leo( Game* owner, Vec2 const& startPos )
	: Entity( owner, startPos )
{
	m_physicsRadius = 0.4f;
	m_cosmeticRadius = 0.5f;
	m_isPushedByWalls = true;
	m_isPushedByEntities = true;
	m_doesPushEntities = true;
	m_isHitByBullets = true;
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

	m_velocity = Vec2(1.f,0.f);

	//Entity* player = m_map->m_entitiesTypeType[ENTITY_TYPE_GOOD_PLAYER][0];

	//if ( m_map->IsAllive( player ) )
	//{
		//if ( m_map->HasLineOfSight( player->m_position, m_position ) )
	//	{
	//		//m_targetPos = Player position;
	//	}
	//}
	//if ( m_targetPos != Vec2 ZERO )
	//{
	//	TurnedTowardPosition( player->m_position, deltaSeconds );

	//	if ( IsPointInsideOrientedSector2D( m_targetPos, m_position, m_orientationDegrees, 90.f, LEO_MAX_VIS) ); //10.f
	//	{
	//		DriveForward( deltaSeconds );

	//		if ( IsPointInsideOrientedSector2D( m_targetPos, m_position, m_orientationDegrees, 10.f, LEO_MAX_VIS ) )
	//		{
	//			TryToShoot();
	//		}
	//	}
	//}
	//else
	//{
	//	Wander
	//}
	m_position += m_velocity * deltaSeconds;
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

void Leo::Shoot()
{

}

void Leo::DriveForward( float deltaSeconds )
{

}

