#include "Game/WeaponDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"

std::map<std::string, WeaponDefinition> WeaponDefinition::s_definitions;

//-----------------------------------------------------------------------------------------------
void WeaponDefinition::InitializeWeaponDefs()
{
	LoadWeaponDefsFromFile( "Data/Definitions/WeaponDefinitions.xml" );
}

//-----------------------------------------------------------------------------------------------
void WeaponDefinition::LoadWeaponDefsFromFile( char const* filePath )
{
	XmlDocument doc;
	doc.LoadFile( filePath );

	XmlElement* root = doc.RootElement();
	if ( !root )
		return;

	XmlUtils xml;
	XmlElement* weaponElem = root->FirstChildElement( "WeaponDefinition" );

	while ( weaponElem )
	{
		WeaponDefinition def;

		def.m_name = xml.ParseXmlAttribute( *weaponElem, "name", "" );
		def.m_damage = xml.ParseXmlAttribute( *weaponElem, "damage", 1 );
		def.m_fireRate = xml.ParseXmlAttribute( *weaponElem, "fireRate", 5.0f );
		def.m_bulletSpeed = xml.ParseXmlAttribute( *weaponElem, "bulletSpeed", 8.0f );
		def.m_spritePath = xml.ParseXmlAttribute( *weaponElem, "sprite", "" );

		if ( def.m_fireRate > 0.f )
		{
			def.m_bulletCooldown = def.m_fireRate;
		}

		s_definitions[def.m_name] = def;

		weaponElem = weaponElem->NextSiblingElement( "WeaponDefinition" );
	}
}

//-----------------------------------------------------------------------------------------------
const WeaponDefinition* WeaponDefinition::GetByName( const std::string& name )
{
	auto it = s_definitions.find( name );
	if ( it == s_definitions.end() )
		return nullptr;
	return &( it->second );
}