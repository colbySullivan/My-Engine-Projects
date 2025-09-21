#pragma once

#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec2.hpp"


//------------------------------------------------------------------------------

constexpr int NUM_DEBRIS_SIDES = 6;
constexpr int NUM_DEBRIS_TRIS = NUM_DEBRIS_SIDES;
constexpr int NUM_DEBRIS_VERTS = 3 * NUM_DEBRIS_TRIS;


//------------------------------------------------------------------------------

class Debris : public Entity
{
public:
	Debris(Game* owner, Vec2 const& startPos);
	~Debris();


	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;

private:
	void InitializeLocalVerts();
	float	m_timeoutTimer = DEBRIS_LIFETIME_SECONDS;

private:
	Vertex m_localVerts[NUM_DEBRIS_VERTS];
};