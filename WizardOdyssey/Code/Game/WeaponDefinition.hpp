#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include <string>
#include <map>

class WeaponDefinition
{
public:
	WeaponDefinition() = default;
	~WeaponDefinition() = default;

	static void InitializeWeaponDefs();
	static void LoadWeaponDefsFromFile( char const* filePath );
	static const WeaponDefinition* GetByName( const std::string& name );

public:
	std::string m_name = "";
	int         m_damage = 1;
	float       m_fireRate = 5.0f;     
	float       m_bulletCooldown = 0.2f;
	float       m_bulletSpeed = 8.0f;
	std::string m_spritePath = "";

	static std::map<std::string, WeaponDefinition> s_definitions;
};