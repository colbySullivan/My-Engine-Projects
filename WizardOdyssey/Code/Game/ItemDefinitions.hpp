#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include <map>
#include <string>

struct ItemStatBonuses
{
	int   m_healing = 0;
	float m_rangeBoost = 0.f;
	float m_meleeBoost = 0.f;
	int   m_armor = 0;
	int   m_magicResistance = 0;
	float m_lifeSteal = 0.f;
	float m_dodge = 0.f;
	float m_speedBoost = 0.f;
};

class ItemDefinitions
{
public:
	static void InitializeItemDefs();
	static void LoadItemDefsFromFile( char const* filePath );
	static const ItemDefinitions* GetByName( const std::string& name );

	std::string     m_name;
	std::string     m_type;
	std::string     m_iconTexture;
	int             m_cost = 0;
	ItemStatBonuses m_statBonuses;

	static std::map<std::string, ItemDefinitions> s_definitions;
};
