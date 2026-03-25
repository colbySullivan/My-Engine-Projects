#pragma once

#include "Game/Game.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Renderer/Texture.hpp"

//-----------------------------------------------------------------------------------------------

struct EulerAngles;
class Game;

//-----------------------------------------------------------------------------------------------
class Actor
{
public:
	Actor( Game* owner, Vec3 start, Vec3 end, float radius, int numSlices );
	virtual ~Actor();

	virtual void Update( float deltaSeconds );
	virtual void Render() const;
	virtual Mat44 GetModelToWorldTransform() const;
public:
	Game* m_game = nullptr;
	Vec3        m_position;
	Vec3        m_velocity;
	EulerAngles	m_orientation;
	EulerAngles m_angularVelocity;
	Rgba8 m_color = Rgba8( 255, 255, 255 );
	Texture* m_texture = nullptr;
	std::vector<Vertex> m_vertexes;
};