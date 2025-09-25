#pragma once

#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex.hpp"


//------------------------------------------------------------------------------

constexpr int NUM_BULLET_TRIS = 2;
constexpr int NUM_BULLET_VERTS = 3 * NUM_BULLET_TRIS;


//------------------------------------------------------------------------------

class Bullet : public Entity
{
public:
	Bullet(Game* owner, Vec2 const& startPos);
	~Bullet();

	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
private:
	void InitializeLocalVerts();

	

private:
	Vertex m_localVerts[NUM_BULLET_VERTS];
};