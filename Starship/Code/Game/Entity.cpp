#include "Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Game/App.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/Debris.hpp"

Entity::Entity(Game* owner, Vec2 const& startPos )
{
	m_game = owner;
	m_position = startPos;
	//m_velocity = velocity;
	//m_health = health;
}

Entity::~Entity()
{

}

void Entity::Update( [[maybe_unused]] float deltaSeconds)
{
	if (m_health == 0 && !m_isDead)
	{
		m_isGarbage = true;
		Die();
	}
}

void Entity::Render() const
{

}

void Entity::DebugRender() const
{
	DebugDrawRing(m_position, m_physicsRadius, 0.2f, Rgba8(255,0,255)); // Outer circle
	DebugDrawRing(m_position, m_cosmeticRadius, 0.2f, Rgba8(0,255,255)); // Inner circle

	Vec2 playerShipPos = g_theApp->m_game->m_playerShip->m_position;
	Vec2 forwardDebugLine = m_position + GetForwardNormal() * m_cosmeticRadius;
	Vec2 rotatedDebugLine = m_position + GetForwardNormal().GetRotatedBy90Degrees() * m_cosmeticRadius;
	Vec2 velocityDebugLine = m_position + 1.f * m_velocity;

	DebugDrawLine(playerShipPos, m_position, 0.2f, Rgba8(50, 50, 50)); // Line to entities
	DebugDrawLine(m_position, forwardDebugLine, 0.2f, Rgba8(255, 0, 0)); // Forward line
	DebugDrawLine(m_position, rotatedDebugLine, 0.2f, Rgba8(0, 255, 0)); // Right line
	DebugDrawLine(m_position, velocityDebugLine, 0.2f, Rgba8(255,255,0)); // Velocity line


}

void Entity::Die()
{
	m_isDead = true;
	g_theApp->m_game->SpawnDebrisCluster(m_position, m_entityColor, m_velocity, m_debrisAmount, m_cosmeticRadius);
	
}

bool Entity::IsOffscreen() const
{
	return (m_position.x > WORLD_SIZE_X + m_cosmeticRadius || m_position.y > WORLD_SIZE_Y + m_cosmeticRadius || m_position.x < 0 - m_cosmeticRadius || m_position.y < 0 - m_cosmeticRadius);
}

Vec2 Entity::GetForwardNormal() const
{
	return Vec2(CosDegrees(m_orientationDegrees), SinDegrees(m_orientationDegrees));
}

