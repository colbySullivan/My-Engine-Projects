#pragma once

#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex.hpp"


//------------------------------------------------------------------------------

constexpr int NUM_INTERACTABLE_VERTS = 192;
constexpr int NUM_INTERACTABLE_TRIS = 64;
constexpr int NUM_INTERACTABLE_SIDES = 64;


//------------------------------------------------------------------------------

class Interactable : public Entity
{
public:
	Interactable(Game* owner, Vec2 const& startPos);
	~Interactable();

	void Update(float deltaSeconds);
	void Render() const;
	Game* m_game = nullptr;
	Vec2        m_position;
	void Die() override;

private:
	void InitializeLocalVerts();

private:
	Vertex m_interactableVerts[NUM_INTERACTABLE_VERTS];
};