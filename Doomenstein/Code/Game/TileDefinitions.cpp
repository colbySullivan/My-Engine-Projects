#include "Game/TileDefinitions.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"

std::map<std::string, TileDefinition> TileDefinition::s_definitions;
//SpriteSheet* g_testSpriteSheet = nullptr;
XmlUtils m_xml2;

void TileDefinition::InitializeTileDefs()
{
	s_definitions.clear();

	XmlDocument doc;
	XmlError eResult = doc.LoadFile( "Data/Definitions/TileDefinitions.xml" );
	if ( eResult == 0 )
	{
		XmlElement* rootElement = doc.RootElement();
		if ( rootElement )
		{
			XmlElement* mapDefElement = rootElement->FirstChildElement( "TileDefinition" );
			while ( mapDefElement )
			{
				std::string name = m_xml2.ParseXmlAttribute( *mapDefElement, "name", "Unnamed" );
				TileDefinition& def = TileDefinition::s_definitions[name];

				def.m_name = name;
				def.m_isSolid = m_xml2.ParseXmlAttribute( *mapDefElement, "isSolid", false );

				std::string colorStr = m_xml2.ParseXmlAttribute( *mapDefElement, "mapImagePixelColor", "255,255,255" );
				def.m_mapImagePixelColor.SetFromText( colorStr.c_str() );

				def.m_floorSpriteCoords = m_xml2.ParseXmlAttribute( *mapDefElement, "floorSpriteCoords", IntVec2( -1, -1 ) );
				def.m_ceilingSpriteCoords = m_xml2.ParseXmlAttribute( *mapDefElement, "ceilingSpriteCoords", IntVec2( -1, -1 ) );
				def.m_wallSpriteCoords = m_xml2.ParseXmlAttribute( *mapDefElement, "wallSpriteCoords", IntVec2( -1, -1 ) );

				mapDefElement = mapDefElement->NextSiblingElement( "TileDefinition" );
			}
		}
	}
 }

TileDefinition::TileDefinition()
{
}

TileDefinition::~TileDefinition()
{
}
