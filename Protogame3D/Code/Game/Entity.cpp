#include "Entity.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Engine.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"

Entity::Entity(Game* owner, Vec3 const& startPos )
{
	m_game = owner;
	m_position = startPos;
	m_startingHealth = m_health;
}

//-----------------------------------------------------------------------------------------------
Entity::~Entity()
{

}

//-----------------------------------------------------------------------------------------------
void Entity::Update( [[maybe_unused]] float deltaSeconds)
{
	if (m_health <= 0 && !m_isDead)
	{
		Die();
	}
}

//-----------------------------------------------------------------------------------------------
void Entity::Render() const
{

}

//-----------------------------------------------------------------------------------------------
void Entity::DebugRender() const
{
	//DebugDrawRing(m_position, m_physicsRadius, 0.2f, Rgba8(255,0,255)); // Outer circle
	//DebugDrawRing(m_position, m_cosmeticRadius, 0.2f, Rgba8(0,255,255)); // Inner circle

	//Vec2 forwardDebugLine = m_position + GetForwardNormal() * m_cosmeticRadius;
	//Vec2 rotatedDebugLine = m_position + GetForwardNormal().GetRotatedBy90Degrees() * m_cosmeticRadius;
	//Vec2 velocityDebugLine = m_position + 1.f * m_velocity;

	//DebugDrawLine(m_position, forwardDebugLine, 0.2f, Rgba8(255, 0, 0)); // Forward line
	//DebugDrawLine(m_position, rotatedDebugLine, 0.2f, Rgba8(0, 255, 0)); // Right line
	//DebugDrawLine(m_position, velocityDebugLine, 0.2f, Rgba8(255,255,0)); // Velocity line


}

//-----------------------------------------------------------------------------------------------
void Entity::Die()
{
	m_isGarbage = true;
	m_isDead = true;
}