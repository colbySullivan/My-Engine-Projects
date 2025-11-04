#include "Game/Leo.hpp"

Leo::Leo( Game* owner, Vec2 const& startPos )
	: Entity( owner, startPos )
{

}

Leo::~Leo()
{
	
}

void Leo::Update( float deltaSeconds )
{
	Entity::Update(deltaSeconds);

	m_velocity = Vec2(0.f,0.f);

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
}

void Leo::Render() const
{

}

void Leo::Shoot()
{

}

void Leo::DriveForward( float deltaSeconds )
{

}

