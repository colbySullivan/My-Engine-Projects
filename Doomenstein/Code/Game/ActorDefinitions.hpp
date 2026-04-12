#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <map>
#include <string>
#include <vector>

class ActorDefinition
{
public:
	ActorDefinition() = default;
	static void InitializeActorDefs();
	static const ActorDefinition* GetByName( const std::string& name );

	// Base Attributes
	std::string m_name;
	std::string m_faction = "NEUTRAL";
	int         m_health = 1;
	bool        m_canBePossessed = true;
	float       m_corpseLifetime = 0.f;
	bool        m_visible = false;

	// Collision
	float       m_physicsRadius = 0.f;
	float       m_physicsHeight = 0.f;
	bool        m_collidesWithWorld = false;
	bool        m_collidesWithActors = false;

	// Projectile / collision extras
	FloatRange  m_damageOnCollide = FloatRange( 0.f, 0.f );
	float       m_impulseOnCollide = 0.f;
	bool        m_dieOnCollide = false;

	// Physics
	bool        m_simulated = false;
	float       m_walkSpeed = 0.f;
	float       m_runSpeed = 0.f;
	float       m_turnSpeed = 0.f;
	float       m_drag = 0.f;
	bool        m_flying = false;
	// AI
	bool        m_aiEnabled = false;

	// Visuals
	Vec2        m_visualSize = Vec2( 1.f, 1.f );
	Vec2        m_visualPivot = Vec2( 0.5f, 0.f );
	std::string m_billboardType = "WorldUpFacing";
	bool        m_renderLit = false;
	bool        m_renderRounded = false;
	std::string m_shaderPath;
	std::string m_spriteSheetPath;
	IntVec2     m_cellCount = IntVec2( 1, 1 );

	// Camera
	float       m_eyeHeight = 0.f;
	float       m_cameraFOV = 60.f;

	// Weapons and Inventory
	std::vector<std::string> m_weaponNames;

	static std::map<std::string, ActorDefinition> s_definitions;
};