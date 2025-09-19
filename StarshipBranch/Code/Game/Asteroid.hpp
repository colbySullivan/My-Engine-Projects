#pragma once

#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


//------------------------------------------------------------------------------

constexpr int NUM_ASTEROID_SIDES = 16;
constexpr int NUM_ASTEROID_TRIS = NUM_ASTEROID_SIDES;
constexpr int NUM_ASTEROID_VERTS = 3 * NUM_ASTEROID_TRIS;

class RandomNumberGenerator;

//------------------------------------------------------------------------------

class Asteroid : public Entity
{
public:
	Asteroid(Game* owner, Vec2 const& startPos);
	~Asteroid();
	
	RandomNumberGenerator g_rng;
	
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;

private:
	void InitializeLocalVerts();

private:
	Vertex m_localVerts[NUM_ASTEROID_VERTS];
};