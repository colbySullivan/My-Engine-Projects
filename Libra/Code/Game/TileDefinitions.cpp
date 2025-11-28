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

				//s_definitions[TileName] = TileDefinition( AABB2( grassMins, grassMaxs ), TileTint, isTileSolid);

				mapDefElement = mapDefElement->NextSiblingElement( "TileDefinition" );
			}
		}
	}

	/*const SpriteDefinition& grassSprite = tilesSpriteSheet->GetSpriteDef(49);
	Vec2 grassMins, grassMaxs;
	grassSprite.GetUVs(grassMins, grassMaxs);
	s_definitions[TILE_TYPE_GRASS].m_uvs = AABB2(grassMins, grassMaxs);
	s_definitions[TILE_TYPE_GRASS].m_isSolid = false;
	s_definitions[TILE_TYPE_GRASS].m_tint = Rgba8( 255, 255, 255, 255 );

	const SpriteDefinition& stoneSprite = tilesSpriteSheet->GetSpriteDef(21);
	Vec2 stoneMins, stoneMaxs;
	stoneSprite.GetUVs(stoneMins, stoneMaxs);
	s_definitions[TILE_TYPE_STONE].m_uvs = AABB2(stoneMins, stoneMaxs);
	s_definitions[TILE_TYPE_STONE].m_isSolid = true;
	s_definitions[TILE_TYPE_STONE].m_tint = Rgba8;

	const SpriteDefinition& mudSprite = tilesSpriteSheet->GetSpriteDef( 45 );
	Vec2 mudMins, mudMaxs;
	mudSprite.GetUVs( mudMins, mudMaxs );
	s_definitions[TILE_TYPE_MUD].m_uvs = AABB2( mudMins, mudMaxs );
	s_definitions[TILE_TYPE_MUD].m_isSolid = false;
	s_definitions[TILE_TYPE_MUD].m_tint = Rgba8( 255, 255, 255, 255 );

	const SpriteDefinition& portalSprite = tilesSpriteSheet->GetSpriteDef( 1 );
	Vec2 portalMins, portalMaxs;
	portalSprite.GetUVs( portalMins, portalMaxs );
	s_definitions[TILE_TYPE_PORTAL].m_uvs = AABB2( portalMins, portalMaxs );
	s_definitions[TILE_TYPE_PORTAL].m_isSolid = false;
	s_definitions[TILE_TYPE_PORTAL].m_tint = Rgba8( 255, 255, 255, 255 );

	const SpriteDefinition& stoneBarrierSprite = tilesSpriteSheet->GetSpriteDef( 13 );
	Vec2 stoneBarrierMins, stoneBarrierMaxs;
	stoneBarrierSprite.GetUVs( stoneBarrierMins, stoneBarrierMaxs );
	s_definitions[TILE_TYPE_STONE_BARRIER].m_uvs = AABB2( stoneBarrierMins, stoneBarrierMaxs );
	s_definitions[TILE_TYPE_STONE_BARRIER].m_isSolid = true;
	s_definitions[TILE_TYPE_STONE_BARRIER].m_tint = Rgba8( 255, 255, 255, 255 );

	const SpriteDefinition& floorStoneSprite = tilesSpriteSheet->GetSpriteDef( 11 );
	Vec2 floorStoneMins, floorStoneMaxs;
	floorStoneSprite.GetUVs( floorStoneMins, floorStoneMaxs );
	s_definitions[TILE_TYPE_STONE_FLOOR].m_uvs = AABB2( floorStoneMins, floorStoneMaxs );
	s_definitions[TILE_TYPE_STONE_FLOOR].m_isSolid = false;
	s_definitions[TILE_TYPE_STONE_FLOOR].m_tint = Rgba8( 255, 255, 255, 255 );

	const SpriteDefinition& startSprite = tilesSpriteSheet->GetSpriteDef( 0 );
	Vec2 startMins, startMaxs;
	startSprite.GetUVs( startMins, startMaxs );
	s_definitions[TILE_TYPE_START].m_uvs = AABB2( startMins, startMaxs );
	s_definitions[TILE_TYPE_START].m_isSolid = false;
	s_definitions[TILE_TYPE_START].m_tint = Rgba8( 255, 255, 255, 255 );

	const SpriteDefinition& brickStoneSprite = tilesSpriteSheet->GetSpriteDef( 16 );
	Vec2 brickStoneMins, brickStoneMaxs;
	brickStoneSprite.GetUVs( brickStoneMins, brickStoneMaxs );
	s_definitions[TILE_TYPE_BRICK_STONE].m_uvs = AABB2( brickStoneMins, brickStoneMaxs );
	s_definitions[TILE_TYPE_BRICK_STONE].m_isSolid = true;
	s_definitions[TILE_TYPE_BRICK_STONE].m_tint = Rgba8( 255, 255, 255, 255 );

	const SpriteDefinition& sandSprite = tilesSpriteSheet->GetSpriteDef( 47 );
	Vec2 sandMins, sandMaxs;
	sandSprite.GetUVs( sandMins, sandMaxs );
	s_definitions[TILE_TYPE_SAND].m_uvs = AABB2( sandMins, sandMaxs );
	s_definitions[TILE_TYPE_SAND].m_isSolid = false;
	s_definitions[TILE_TYPE_SAND].m_tint = Rgba8( 255, 255, 255, 255 );*/
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
