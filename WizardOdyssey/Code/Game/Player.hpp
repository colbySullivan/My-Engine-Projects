#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "SpriteAnimationDefinition.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/Timer.hpp"

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

private:
	void InitializePlayerVerts();
	void UpdateFromKeyboard( float deltaSeconds );
	void UpdateFromController( float deltaSeconds );
	void SetCurrentAnimGroup( const std::string& groupName );
	void InitializeTurretVerts();
	void RenderPlayer() const;
	void RenderGun() const;

	void UpdateLinearMovement();
	void UpdateTurretOrientation();

private:
	std::vector<Vertex>		m_playerVerts;
	std::vector<Vertex>		m_turretVerts;
	float		m_thrustFraction = -5.f;
	bool		m_isMoving = false;

	// Animations
	const SpriteAnimationDefinition* m_spriteAnimationDef = nullptr;
	const SpriteAnimationGroupDefinition* m_currentAnimGroup = nullptr;
	double m_currentAnimStartTime = 0.0;
	SpriteSheet* m_currentSpriteSheet = nullptr;
	Timer* m_animTimer = nullptr;
	void InitializePlayerSpriteSheet();
};