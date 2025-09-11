#include "Entity.hpp"

Entity::Entity(Game* owner, Vec2 const& startPos)
{
	m_game = owner;
	m_position = startPos;
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

}

bool Entity::IsOffscreen() const
{
	return false;
}

Vec2 Entity::GetForwardNormal() const
{
	return Vec2(0, 0);
}

