#pragma once

#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include <string>
#include <map>

struct ActorCollisionDef
{
	float m_physicRadius = 0.f;
	float m_cosmeticRadius = 0.f;
};

struct ActorPhysicsDef
{
	float m_walkSpeed = 0.f;
	float m_runSpeed = 0.f;
};

struct ActorVisualDef
{
	Vec2 m_visualSize = Vec2( 1.f, 1.f );
};


struct ActorSoundDef
{
	std::string m_hurtSoundPath = "";
	std::string m_deathSoundPath = "";
};

struct GameStats
{
	int     m_healing = 0;
	float   m_rangeBoost = 1.0f;
	float   m_meleeBoost = 1.0f;
	int     m_armor = 0;
	int     m_magicResistance = 0;
	float   m_lifeSteal = 0.0f;
	float   m_dodge = 0.0f;
	float   m_speedBoost = 1.0f;
};

class ActorDefinitions
{
public:
	ActorDefinitions() = default;
	~ActorDefinitions() = default;

	static void InitializeActorDefs();
	static void LoadActorDefsFromFile( char const* filePath );
	static const ActorDefinitions* GetByName( const std::string& name );

public:
	std::string m_name = "";
	int         m_health = 100;
	bool        m_visible = true;

	ActorCollisionDef m_collision;
	ActorPhysicsDef   m_physics;
	ActorVisualDef	  m_visuals;
	ActorSoundDef     m_sounds;
	GameStats		  m_gameStats;

	static std::map<std::string, ActorDefinitions> s_definitions;
};