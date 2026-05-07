#include "PowerUpDefinitions.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

std::map<std::string, PowerUpDefinition> PowerUpDefinition::s_definitions;

//-----------------------------------------------------------------------------------------------
static void LoadPowerUpDefsFromFile( char const* filePath )
{
	XmlDocument doc;
	doc.LoadFile( filePath );

	XmlElement* root = doc.RootElement();
	if ( !root )
	{
		return;
	}

	XmlElement* powerupElem = root->FirstChildElement( "Powerup" );
	XmlUtils xml;

	while ( powerupElem )
	{
		std::string name = xml.ParseXmlAttribute( *powerupElem, "name", "" );
		if ( name.empty() )
		{
			ERROR_AND_DIE( "PowerUp definition missing name attribute" );
		}

		PowerUpDefinition& def = PowerUpDefinition::s_definitions[name];
		def.m_name = name;
		def.m_type = PowerUpDefinition::GetTypeFromName( name );
		def.m_percentage = xml.ParseXmlAttribute( *powerupElem, "percentage", FloatRange( 1.0f, 10.0f ) );

		powerupElem = powerupElem->NextSiblingElement( "Powerup" );
	}

}


//------------------------------------------------------------------------------
void PowerUpDefinition::InitializePowerUpDefs()
{
	LoadPowerUpDefsFromFile( "Data/Definitions/MayhemDefinitions.xml" );
}

//-----------------------------------------------------------------------------------------------
const PowerUpDefinition* PowerUpDefinition::GetByName( const std::string& name )
{
	std::map<std::string, PowerUpDefinition>::const_iterator found = s_definitions.find( name );
	if ( found == s_definitions.end() )
	{
		return nullptr;
	}
	return &found->second;
}

//-----------------------------------------------------------------------------------------------
PowerUpType PowerUpDefinition::GetTypeFromName( const std::string& name )
{
	if ( name == "SpeedBoost" )
	{
		return PowerUpType::SPEED_BOOST;
	}
	else if ( name == "ShootSpeed" )
	{
		return PowerUpType::SHOOT_SPEED;
	}
	else if ( name == "HealthBoost" )
	{
		return PowerUpType::HEALTH_BOOST;
	}

	return PowerUpType::INVALID;
}