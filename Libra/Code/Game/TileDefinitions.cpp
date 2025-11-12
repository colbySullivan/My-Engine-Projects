#include "Game/TileDefinitions.hpp"
#include "Engine/Core/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"

std::vector<TileDefinition> TileDefinition::s_definitions;
//SpriteSheet* g_testSpriteSheet = nullptr;

void TileDefinition::InitializeTileDefs()
{
	s_definitions.resize(NUM_TILE_TYPES);

	Texture* spriteSheetTexture = g_engine->m_render->CreateOrGetTextureFromFile("Data/Textures/Terrain_8x8.png");
	SpriteSheet* tilesSpriteSheet = new SpriteSheet(*spriteSheetTexture, IntVec2(8, 8));

	const SpriteDefinition& grassSprite = tilesSpriteSheet->GetSpriteDef(49);
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
	s_definitions[TILE_TYPE_STONE].m_tint = Rgba8( 255, 255, 255, 255 );

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
