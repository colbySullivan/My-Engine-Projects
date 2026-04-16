#include "Game/WeaponDefinition.hpp"

//-----------------------------------------------------------------------------------------------
std::map<std::string, WeaponDefinition> WeaponDefinition::s_definitions;

//-----------------------------------------------------------------------------------------------
void WeaponDefinition::InitializeWeaponDefs()
{
	XmlDocument doc;
	doc.LoadFile( "Data/Definitions/WeaponDefinitions.xml" );

	XmlElement* root = doc.RootElement();
	if ( !root )
	{
		return;
	}

	XmlElement* weaponElem = root->FirstChildElement( "WeaponDefinition" );
	XmlUtils xml;

	while ( weaponElem )
	{
		WeaponDefinition def;

		def.m_name       = xml.ParseXmlAttribute( *weaponElem, "name",       std::string( "Unnamed" ) );
		def.m_refireTime = xml.ParseXmlAttribute( *weaponElem, "refireTime", 0.f );

		def.m_rayCount   = xml.ParseXmlAttribute( *weaponElem, "rayCount",   0 );
		def.m_rayCone    = xml.ParseXmlAttribute( *weaponElem, "rayCone",    0.f );
		def.m_rayRange   = xml.ParseXmlAttribute( *weaponElem, "rayRange",   0.f );
		def.m_rayDamage  = xml.ParseXmlAttribute( *weaponElem, "rayDamage",  FloatRange( 0.f, 1.f ), '~' );
		def.m_rayImpulse = xml.ParseXmlAttribute( *weaponElem, "rayImpulse", 0.f );

		def.m_projectileCount       = xml.ParseXmlAttribute( *weaponElem, "projectileCount", 0 );
		def.m_projectileActorName   = xml.ParseXmlAttribute( *weaponElem, "projectileActor", std::string( "" ) );
		def.m_projectileConeDegrees = xml.ParseXmlAttribute( *weaponElem, "projectileCone",  0.f );
		def.m_projectileSpeed       = xml.ParseXmlAttribute( *weaponElem, "projectileSpeed", 0.f );

		def.m_meleeCount    = xml.ParseXmlAttribute( *weaponElem, "meleeCount",  0 );
		def.m_meleeArcDegrees = xml.ParseXmlAttribute( *weaponElem, "meleeArc",   0.f );
		def.m_meleeRange    = xml.ParseXmlAttribute( *weaponElem, "meleeRange",  0.f );
		def.m_meleeDamage   = xml.ParseXmlAttribute( *weaponElem, "meleeDamage", FloatRange( 0.f, 1.f ), '~');
		def.m_meleeImpulse  = xml.ParseXmlAttribute( *weaponElem, "meleeImpulse", 0.f );


		s_definitions[def.m_name] = def;

		weaponElem = weaponElem->NextSiblingElement( "WeaponDefinition" );
	}
}

//-----------------------------------------------------------------------------------------------
const WeaponDefinition* WeaponDefinition::GetByName( const std::string& name )
{
	std::map<std::string, WeaponDefinition>::iterator it = s_definitions.find( name );
	if ( it == s_definitions.end() )
	{
		return nullptr;
	}

	const WeaponDefinition& foundDefinition = it->second;
	return &foundDefinition;
}