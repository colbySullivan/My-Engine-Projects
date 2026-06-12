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

	Texture* spriteSheetTexture = g_engine->m_render->CreateOrGetTextureFromFile("Data/Textures/Terrain_8x8.png");
	SpriteSheet* tilesSpriteSheet = new SpriteSheet(*spriteSheetTexture, IntVec2(8, 8));

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
				std::string TileName = m_xml2.ParseXmlAttribute( *mapDefElement, "name", "InvalidTileName");
				bool isTileSolid = m_xml2.ParseXmlAttribute( *mapDefElement, "isSolid", false );
				IntVec2 SpriteCoords = m_xml2.ParseXmlAttribute( *mapDefElement, "spriteCoords", IntVec2( 0, 0 ) );
				Rgba8 TileTint = m_xml2.ParseXmlAttribute( *mapDefElement, "tint", Rgba8( 255, 255, 255, 255 ) );

				s_definitions[TileName].m_isSolid = isTileSolid;
				s_definitions[TileName].m_tint = TileTint;

				const SpriteDefinition& grassSprite = tilesSpriteSheet->GetSpriteDefAtCoords( SpriteCoords );
				Vec2 grassMins, grassMaxs;
				grassSprite.GetUVs( grassMins, grassMaxs );
				s_definitions[TileName].m_uvs = AABB2( grassMins, grassMaxs );

				mapDefElement = mapDefElement->NextSiblingElement( "TileDefinition" );
			}
		}
	}
 }

TileDefinition::TileDefinition()
{

}

TileDefinition::TileDefinition( AABB2 uvs, Rgba8 tint, bool isSolid )
	: m_uvs( uvs )
	, m_tint( tint )
	, m_isSolid( isSolid )
{
}

TileDefinition::~TileDefinition()
{

}
