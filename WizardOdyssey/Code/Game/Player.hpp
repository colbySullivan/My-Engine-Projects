#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/Timer.hpp"
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Game/SpriteAnimationDefinition.hpp"
#include "Game/Weapon.hpp"


//------------------------------------------------------------------------------
class Game;

//------------------------------------------------------------------------------

class Player : public Entity
{
public:
	Player( Game* owner, Vec2 const& startPos, float orientationDegrees, EntityFaction faction, Map* map, EntityType type );
	~Player();

	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;
	virtual bool IsPlayer() const override;

	int					m_lives = 1;
	void Respawn() override;
	void PlayerHit();
	void AddWeapon( std::string weaponName );
	Strings GetStats();

private:
	void InitializePlayerVerts();
	void UpdateFromKeyboard( float deltaSeconds );
	void UpdateFromController( float deltaSeconds );
	void SetCurrentAnimGroup( const std::string& groupName );
	void InitializeTurretVerts();
	//void RenderPlayer() const;
	void RenderGun() const;

	void UpdateLinearMovement();
	void UpdateTurretOrientation();

private:
	std::vector<Vertex>		m_playerVerts;
	std::vector<Vertex>		m_turretVerts;
	float					m_thrustFraction = -5.f;
	bool					m_isMoving = false;
	Timer*					m_hitTimer = nullptr;
	static constexpr int   MAX_WEAPONS = 64;
	static constexpr float ORBIT_RADIUS = 1.0f;

	int m_numEquipedWeapons = 0;
	Weapon* m_weapons[MAX_WEAPONS] = {};
};