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

void Entity::createDebris(int numDebrisToCreate, Vec2 velocity)
{
	for (int debrisIndex = 0; debrisIndex < numDebrisToCreate; ++debrisIndex)
	{
		Vec2 debrisVelocity = GetForwardNormal().GetRotatedByDegrees(g_rng.RollRandomFloatInRange(-45.f, 45.f)) * velocity;
		Debris* debris = new Debris(m_game, m_position);
		debris->m_velocity = debrisVelocity;
	}
}

bool Entity::IsOffscreen() const
{
	return (m_position.x > WORLD_SIZE_X + m_cosmeticRadius || m_position.y > WORLD_SIZE_Y + m_cosmeticRadius || m_position.x < 0 - m_cosmeticRadius || m_position.y < 0 - m_cosmeticRadius);
}

Vec2 Entity::GetForwardNormal() const
{
	return Vec2(CosDegrees(m_orientationDegrees), SinDegrees(m_orientationDegrees));
}

