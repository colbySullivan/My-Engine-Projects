#include "PlayerShip.hpp"
#include <Engine/Core/Vertex.hpp>

PlayerShip::PlayerShip(Vec2 const& startPosition, Vec2 const& startingVelocity)
	: m_position(startPosition)
	, m_velocity(startingVelocity)
{
}

void PlayerShip::Update(float deltaSeconds)
{
	m_position += m_velocity * deltaSeconds;
}