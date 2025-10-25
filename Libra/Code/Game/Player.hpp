#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex.hpp"

//------------------------------------------------------------------------------
class Game;

//------------------------------------------------------------------------------

class Player : public Entity
{
public:
	Player(Game* owner, Vec2 const& startPos);
	~Player();

	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	//void Die() override;

	int					m_lives = 4;

private:
	void InitializeLocalVerts();
	void UpdateFromKeyboard(float deltaSeconds);
	//void Shoot();
	void Respawn();

	void UpdateFromController(float deltaSeconds);

private:
	Vertex		m_localVerts[NUM_VERTS];
	float		m_thrustFraction = -5.f;
};