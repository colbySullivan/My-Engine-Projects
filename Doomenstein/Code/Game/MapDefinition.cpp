#include "Game/MapDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include <map>

XmlUtils m_xml;
std::map<std::string, MapDefinition*> MapDefinition::s_definitions;

//-----------------------------------------------------------------------------------------------
void MapDefinition::InitializeMapDefs()
{
	XmlDocument doc;
	XmlError eResult = doc.LoadFile( "Data/Definitions/MapDefinitions.xml" );
	if ( eResult == 0 )
	{
		XmlElement* rootElement = doc.RootElement();
		if ( rootElement )
		{
			XmlElement* mapDefElement = rootElement->FirstChildElement( "MapDefinition" );
			while ( mapDefElement )
			{
				IntVec2 dimensions = m_xml.ParseXmlAttribute( *mapDefElement, "dimensions", IntVec2( 25, 25 ) );
				std::string name = m_xml.ParseXmlAttribute( *mapDefElement, "name", "TestMap" );
				std::string imagePath = m_xml.ParseXmlAttribute( *mapDefElement, "image", "Data/Maps/TestMap.png" );
				std::string shaderPath = m_xml.ParseXmlAttribute( *mapDefElement, "shader", "Data/Shaders/Diffuse" );
				std::string texturePath = m_xml.ParseXmlAttribute( *mapDefElement, "spriteSheetTexture", "Data/Images/Terrain_8x8.png" );
				IntVec2 cellCount = m_xml.ParseXmlAttribute( *mapDefElement, "spriteSheetCellCount", IntVec2( 0, 0 ) );

				//XmlElement* inventoryElem = mapDefElement->FirstChildElement( "SpawnInfos" );
				//if ( inventoryElem )
				//{
				//	XmlElement* spawnElem = inventoryElem->FirstChildElement( "SpawnInfo" );
				//	while ( spawnElem )
				//	{
				//		SpawnInfo spawnI;
				//		spawnI.m_name = m_xml.ParseXmlAttribute( *mapDefElement, "actor", "nothing" );
				//		spawnI.m_spawnLocation = m_xml.ParseXmlAttribute( *mapDefElement, "position", Vec3( 0.f, 0.f, 0.f ) );
				//		if ( spawnI.m_name != "nothing" )
				//		{
				//			//m_spawnInfo.push_back( spawnI );
				//			spawnI.m_spawnLocation = Vec3::ZERO;
				//		}
				//		spawnElem = spawnElem->NextSiblingElement( "SpawnInfo" );

				//	}
				//}

				MapDefinition* mapDef = CreateMapDef( name, imagePath, shaderPath, texturePath, cellCount );
				s_definitions[mapDef->m_name] = mapDef;
				mapDefElement = mapDefElement->NextSiblingElement( "MapDefinition" );
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
const MapDefinition* MapDefinition::GetByName( const std::string& name )
{
	auto it = s_definitions.find( name );
	if ( it == s_definitions.end() )
	{
		return nullptr;
	}
	return it->second;
}

//-----------------------------------------------------------------------------------------------
MapDefinition* MapDefinition::CreateMapDef( std::string name, std::string imagePath, std::string  shaderPath, std::string  texturePath, IntVec2 cellCount )
{
	MapDefinition* mapDef = new MapDefinition;
	mapDef->m_name = name;
	mapDef->m_image = Image( imagePath.c_str() );
	mapDef->m_shader = g_engine->m_render->CreateOrGetShader( shaderPath.c_str(), VertexType::VERTEX_PCUTBN );
	mapDef->m_spriteSheetCellCount = cellCount;
	mapDef->m_spriteSheetTexture = g_engine->m_render->CreateOrGetTextureFromFile( texturePath.c_str() );

	return mapDef;
}