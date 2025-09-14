#include "Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Rgba8.hpp"

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

void Entity::Update(float deltaSeconds)
{

}

void Entity::Render() const
{

}

void Entity::DebugRender() const
{
	DebugDrawRing(m_position, m_physicsRadius, 0.2f, Rgba8(255,0,255)); // Outer
	DebugDrawRing(m_position, m_cosmeticRadius, 0.2f, Rgba8(0,255,255)); // Outer
}

bool Entity::IsOffscreen() const
{
	return (m_position.x > WORLD_SIZE_X || m_position.y > WORLD_SIZE_Y);
}

Vec2 Entity::GetForwardNormal() const
{
	return Vec2(CosDegrees(m_orientationDegrees), SinDegrees(m_orientationDegrees));
}

