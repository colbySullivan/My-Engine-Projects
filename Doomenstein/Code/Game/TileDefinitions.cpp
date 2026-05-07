#include "Game/TileDefinitions.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"

//-----------------------------------------------------------------------------------------------
std::map<std::string, TileDefinition> TileDefinition::s_definitions;

//-----------------------------------------------------------------------------------------------
static void LoadTileDefsFromFile( char const* filePath )
{
	XmlDocument doc;
	doc.LoadFile( filePath );

	XmlElement* rootElement = doc.RootElement();
	if ( !rootElement )
	{
		return;
	}

	XmlElement* tileElem = rootElement->FirstChildElement( "TileDefinition" );
	XmlUtils xml;

	while ( tileElem )
	{
		std::string name = xml.ParseXmlAttribute( *tileElem, "name", std::string( "Unnamed" ) );
		TileDefinition& def = TileDefinition::s_definitions[name];

		def.m_name = name;
		def.m_isSolid = xml.ParseXmlAttribute( *tileElem, "isSolid", false );

		std::string colorStr = xml.ParseXmlAttribute( *tileElem, "mapImagePixelColor", std::string( "255,255,255" ) );
		def.m_mapImagePixelColor.SetFromText( colorStr.c_str() );

		def.m_floorSpriteCoords = xml.ParseXmlAttribute( *tileElem, "floorSpriteCoords", IntVec2( -1, -1 ) );
		def.m_ceilingSpriteCoords = xml.ParseXmlAttribute( *tileElem, "ceilingSpriteCoords", IntVec2( -1, -1 ) );
		def.m_wallSpriteCoords = xml.ParseXmlAttribute( *tileElem, "wallSpriteCoords", IntVec2( -1, -1 ) );

		tileElem = tileElem->NextSiblingElement( "TileDefinition" );
	}
}

//-----------------------------------------------------------------------------------------------
void TileDefinition::InitializeTileDefs()
{
	s_definitions.clear();
	LoadTileDefsFromFile( "Data/Definitions/TileDefinitionsZombies2.xml" );
}

//-----------------------------------------------------------------------------------------------
const TileDefinition* TileDefinition::GetByName( const std::string& name )
{
	std::map<std::string, TileDefinition>::iterator it = s_definitions.find( name );
	if ( it == s_definitions.end() )
	{
		return nullptr;
	}

	const TileDefinition& foundDefinition = it->second;
	return &foundDefinition;
}