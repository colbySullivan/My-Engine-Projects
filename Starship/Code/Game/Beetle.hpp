#pragma once

#pragma once

#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


//------------------------------------------------------------------------------

constexpr int NUM_BEETLE_SIDES = 4;
constexpr int NUM_BEETLE_TRIS = NUM_BEETLE_SIDES;
constexpr int NUM_BEETLE_VERTS = 6;

class RandomNumberGenerator;

//------------------------------------------------------------------------------

class Beetle : public Entity
{
public:
	Beetle(Game* owner, Vec2 const& startPos);
	~Beetle();

	RandomNumberGenerator g_rng;

	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;

private:
	void InitializeLocalVerts();
	void WrapAroundScreen();
	void spawnRandomEdge();

private:
	Vertex m_localVerts[NUM_BEETLE_VERTS];
};