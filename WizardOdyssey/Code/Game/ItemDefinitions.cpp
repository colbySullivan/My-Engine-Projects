#include "Game/ItemDefinitions.hpp"

std::map<std::string, ItemDefinitions> ItemDefinitions::s_definitions;

//-----------------------------------------------------------------------------------------------
void ItemDefinitions::InitializeItemDefs()
{
	LoadItemDefsFromFile( "Data/Definitions/ItemDefinitions.xml" );
}

//-----------------------------------------------------------------------------------------------
void ItemDefinitions::LoadItemDefsFromFile( char const* filePath )
{
	XmlDocument doc;
	doc.LoadFile( filePath );

	XmlElement* root = doc.RootElement();
	if ( !root )
		return;

	XmlUtils xml;
	XmlElement* itemElem = root->FirstChildElement( "ItemDefinition" );

	while ( itemElem )
	{
		ItemDefinitions def;

		def.m_name = xml.ParseXmlAttribute( *itemElem, "name", "" );
		def.m_type = xml.ParseXmlAttribute( *itemElem, "type", "" );
		def.m_iconTexture = xml.ParseXmlAttribute( *itemElem, "iconTexture", "" );
		def.m_cost = xml.ParseXmlAttribute( *itemElem, "cost", 0 );

		def.m_statBonuses.m_healing = xml.ParseXmlAttribute( *itemElem, "bonusHealing", 0 );
		def.m_statBonuses.m_rangeBoost = xml.ParseXmlAttribute( *itemElem, "bonusRangeBoost", 0.f );
		def.m_statBonuses.m_meleeBoost = xml.ParseXmlAttribute( *itemElem, "bonusMeleeBoost", 0.f );
		def.m_statBonuses.m_armor = xml.ParseXmlAttribute( *itemElem, "bonusArmor", 0 );
		def.m_statBonuses.m_magicResistance = xml.ParseXmlAttribute( *itemElem, "bonusMagicResistance", 0 );
		def.m_statBonuses.m_lifeSteal = xml.ParseXmlAttribute( *itemElem, "bonusLifeSteal", 0.f );
		def.m_statBonuses.m_dodge = xml.ParseXmlAttribute( *itemElem, "bonusDodge", 0.f );
		def.m_statBonuses.m_speedBoost = xml.ParseXmlAttribute( *itemElem, "bonusSpeedBoost", 0.f );

		s_definitions[def.m_name] = def;

		itemElem = itemElem->NextSiblingElement( "ItemDefinition" );
	}
}

//-----------------------------------------------------------------------------------------------
const ItemDefinitions* ItemDefinitions::GetByName( const std::string& name )
{
	auto it = s_definitions.find( name );
	if ( it == s_definitions.end() )
		return nullptr;
	return &( it->second );
}