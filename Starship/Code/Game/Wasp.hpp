#pragma once

#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


//------------------------------------------------------------------------------

constexpr int NUM_WASP_SIDES = 4;
constexpr int NUM_WASP_TRIS = NUM_WASP_SIDES;
constexpr int NUM_WASP_VERTS = 6;

class RandomNumberGenerator;

//------------------------------------------------------------------------------

class Wasp : public Entity
{
public:
	Wasp(Game* owner, Vec2 const& startPos);
	~Wasp();

	RandomNumberGenerator g_rng;

	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;

private:
	void InitializeLocalVerts();
	void WrapAroundScreen();
	void spawnRandomEdge();

private:
	Vertex m_localVerts[NUM_WASP_VERTS];
};