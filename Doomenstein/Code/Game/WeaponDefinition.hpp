#pragma once

#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/FloatRange.hpp"
#include <map>
#include <string>

//-----------------------------------------------------------------------------------------------
class WeaponDefinition
{
public:
	WeaponDefinition() = default;

	static void InitializeWeaponDefs();
	static const WeaponDefinition* GetByName( const std::string& name );

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

public:
	static std::map<std::string, WeaponDefinition> s_definitions;
};