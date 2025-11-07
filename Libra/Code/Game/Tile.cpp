#include "Game/Tile.hpp"
#include "Game/TileDefinitions.hpp"

TileDef TileDef::s_tileDefs[NUM_TILE_TYPE];

//-----------------------------------------------------------------------------------------------
void TileDef::InitializeTileDefs()
{
	s_tileDefs[TILE_TYPE_GRASS].m_isSolid = false;
	s_tileDefs[TILE_TYPE_STONE].m_isSolid = true;

	//s_tileDefs[TILE_TYPE_STONE].m_UVs = g_terrainSpriteSheet.GetUVsForSpriteCoords( 2, 1 );

	//for (int tileType = 0; tileType < NUM_TILE_TYPE ; tileType++)
	//{
	//	TileDef const& tileDef = s_tileDefs[tileType];
	//	//GUARANTEE_OR_DIE( tileDef.m_UVs == AABB2(0.f,0.f,1.f,1.f), "TODO");
	//}
}

//-----------------------------------------------------------------------------------------------
Tile::Tile()
{

}

//-----------------------------------------------------------------------------------------------
Tile::~Tile()
{

}

bool Tile::IsSolid() const
{
	TileDef const& myDef = TileDef::s_tileDefs[m_type];
	return myDef.m_isSolid;
}
