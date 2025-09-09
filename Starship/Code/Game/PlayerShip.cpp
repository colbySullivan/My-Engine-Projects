#include "PlayerShip.hpp"
#include <Engine/Core/Vertex.hpp>
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "App.hpp";


PlayerShip::PlayerShip(Vec2 const& startPosition, Vec2 const& startingVelocity)
	: m_position(startPosition)
	, m_velocity(startingVelocity)
{
}

void PlayerShip::Update(float deltaSeconds)
{
	m_position += m_velocity * deltaSeconds;
}

void PlayerShip::Render() const
{
	Vertex shipVerts[3] = {
		Vertex(Vec3(m_position.x + 4.f, m_position.y, 0.f), Rgba8(255, 255, 255, 255), Vec2(0.0f, 0.0f)),		// Ship Nose
		Vertex(Vec3(m_position.x + -2.f, m_position.y + 2.f, 0.f), Rgba8(0, 127, 255, 255), Vec2(0.0f, 0.0f)),	// Ship Left Wing 
		Vertex(Vec3(m_position.x + -2.f, m_position.y + -2.f, 0.f), Rgba8(0, 0, 0, 255), Vec2(0.0f, 0.0f))		// Ship Right Wing
	};
	g_engine->m_render->DrawVertexArray( 3, shipVerts );
}
