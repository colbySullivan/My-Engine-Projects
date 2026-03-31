#pragma once

#include "Game/Game.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/Vec3.hpp"

//-----------------------------------------------------------------------------------------------

enum ActorType
{
	ACTOR,
	PROJECTILE,
	ActorTypeCount
};

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
	void UpdateMove();
	virtual Mat44 GetModelToWorldTransform() const;
public:
	Game* m_game = nullptr;
	Vec3        m_position;
	Vec3        m_velocity;
	EulerAngles	m_orientation = EulerAngles( 0.f, 0.f, 0.f );
	EulerAngles m_angularVelocity;
	Rgba8 m_color = Rgba8( 255, 255, 255 );
	Texture* m_texture = nullptr;
	std::vector<Vertex> m_vertexes;
	float	m_radius;
	bool	m_controlledByPlayer = false;
	ActorType m_actorType = ACTOR;
private:
	void ApplyMovement( Vec3 localMoveDir, float speed, float deltaSeconds );
};