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
	//s_definitions[TILE_TYPE_GRASS].m_uvs = AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) );

	const SpriteDefinition& stoneSprite = tilesSpriteSheet->GetSpriteDef(21);
	Vec2 stoneMins, stoneMaxs;
	stoneSprite.GetUVs(stoneMins, stoneMaxs);
	s_definitions[TILE_TYPE_STONE].m_uvs = AABB2(stoneMins, stoneMaxs);
	s_definitions[TILE_TYPE_STONE].m_isSolid = true;
	s_definitions[TILE_TYPE_STONE].m_tint = Rgba8( 255, 255, 255, 255 );
	//s_definitions[TILE_TYPE_STONE].m_uvs = AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) );
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
