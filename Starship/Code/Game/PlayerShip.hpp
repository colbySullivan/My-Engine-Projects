#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex.hpp"

//------------------------------------------------------------------------------

constexpr int NUM_SHIP_TRIS = 5;
constexpr int NUM_SHIP_VERTS = 3 * NUM_SHIP_TRIS;


//------------------------------------------------------------------------------

class PlayerShip : public Entity
{
public:
	PlayerShip(Game* owner, Vec2 const& startPos); //TODO update this to take in controller id for multiplayer
	~PlayerShip();

	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;

	static void InitializeLocalPlayerShipsVerts( Vertex* vertArray );

	int m_lives = 4;

private:
	void InitializeLocalVerts();
	void UpdateFromKeyboard(float deltaSeconds);
	void BounceOffWalls();
	void Respawn();

	void UpdateFromController(float deltaSeconds);

private:
	Vertex		m_localVerts[NUM_SHIP_VERTS];
	bool        m_isTurningLeft = false;
	bool        m_isTurningRight = false;
	bool        m_isThrusting = false;
	float		m_thrustFraction;
	bool		m_canTriggerShoot = false; // Handle delayed shoot
};