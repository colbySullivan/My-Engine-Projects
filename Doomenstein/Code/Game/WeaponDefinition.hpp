#pragma once

#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/Vec2.hpp"
#include <map>
#include <string>
#include <vector>

//-----------------------------------------------------------------------------------------------
struct WeaponAnimationDefinition
{
	std::string m_name;
	std::string m_shader;
	std::string m_spriteSheet;
	IntVec2 m_cellCount;
	float m_secondsPerFrame = 0.1f;
	int m_startFrame = 0;
	int m_endFrame = 0;
};

//-----------------------------------------------------------------------------------------------
struct WeaponHUDDefinition
{
	std::string m_shader;
	std::string m_baseTexture;
	std::string m_reticleTexture;
	Vec2 m_reticleSize = Vec2(0.f, 0.f);
	Vec2 m_spriteSize = Vec2(0.f, 0.f);
	Vec2 m_spritePivot = Vec2(0.5f, 0.5f);
	std::vector<WeaponAnimationDefinition> m_animations;
};

//-----------------------------------------------------------------------------------------------
struct WeaponSoundDefinition
{
	std::string m_soundName;
	std::string m_filePath;
	float		m_volume;
};

//-----------------------------------------------------------------------------------------------
class WeaponDefinition
{
public:
	WeaponDefinition() = default;

	static void InitializeWeaponDefs();
	static const WeaponDefinition* GetByName( const std::string& name );
	const WeaponAnimationDefinition* GetAnimationByName( const std::string& name ) const;
	const WeaponSoundDefinition* GetSoundByName( const std::string& name ) const;
public:
	std::string m_name;
	float       m_refireTime = 0.f;

	int         m_rayCount   = 0;
	float       m_rayCone    = 0.f;
	float       m_rayRange   = 0.f;
	FloatRange  m_rayDamage  = FloatRange( 0.f, 0.f );
	float       m_rayImpulse = 0.f;

	int         m_projectileCount      = 0;
	std::string m_projectileActorName;   
	float       m_projectileConeDegrees = 0.f;
	float       m_projectileSpeed       = 0.f;

	int         m_meleeCount    = 0;
	float       m_meleeArcDegrees = 0.f;
	float       m_meleeRange    = 0.f;
	FloatRange  m_meleeDamage   = FloatRange( 0.f, 0.f );
	float       m_meleeImpulse  = 0.f;

	WeaponHUDDefinition m_hud;
	std::vector<WeaponSoundDefinition> m_sounds;

public:
	static std::map<std::string, WeaponDefinition> s_definitions;
};