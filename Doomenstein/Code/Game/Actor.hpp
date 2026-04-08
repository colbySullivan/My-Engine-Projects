#pragma once

#include "Game/Game.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Game/ActorHandle.hpp"
#include "Game/Map.hpp"

//-----------------------------------------------------------------------------------------------

enum ActorType
{
	ACTOR,
	PROJECTILE,
	ActorTypeCount
};

//-----------------------------------------------------------------------------------------------
struct ActorDef
{
	
};

struct EulerAngles;
class Game;
class Map;

//-----------------------------------------------------------------------------------------------
class Actor
{
public:
	Actor( Game* owner, Vec3 start, Vec3 end, float radius, int numSlices );
	Actor( Game* owner, Vec3 spawn, EulerAngles startOrientation );
	virtual ~Actor();

	virtual void Update( float deltaSeconds );
	virtual void Render() const;
	void UpdateMove();
	void SetPosXY( float x, float y );
	virtual Mat44 GetModelToWorldTransform() const;

public:
	ActorHandle	m_actorHandle;
	ActorDef*	m_actorDef;
	Map*		m_map;
	Vec3        m_position;
	EulerAngles	m_orientation = EulerAngles( 0.f, 0.f, 0.f );
	Vec3		m_velocity;
	Vec3		m_acceleration;
	std::vector<Vertex> m_vertexes;
	// Weapons 
	Actor*		m_owner;
	bool		m_isDead = false;
	int			m_health;
	Actor*		m_controller;
	Actor*		m_controllerAI;


	EulerAngles m_angularVelocity;
	Rgba8 m_color = Rgba8( 255, 255, 255 );
	Texture* m_texture = nullptr;
	Game* m_game = nullptr;
	Vec3        m_start;
	Vec3        m_end;
	float		m_radius;
	float		m_height;
	bool		m_controlledByPlayer = false;
	bool		m_canBePushed = false;
	ActorType	m_actorType = ACTOR;

private:
	void ApplyMovement( Vec3 localMoveDir, float speed, float deltaSeconds );
};