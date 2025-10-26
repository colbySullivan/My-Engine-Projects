#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Engine.hpp"

//------------------------------------------------------------------------------
Map::Map(IntVec2 dimensions)
	: m_dimensions(dimensions)
{
	BuildMapTiles();
}

//------------------------------------------------------------------------------
int Map::GetTileIndexForTileCoords(IntVec2 tileCoords) const
{
	return (tileCoords.y * m_dimensions.x) + tileCoords.x;
}

//------------------------------------------------------------------------------
IntVec2 Map::GetTileCoordsForIndex(int tileCoords) const
{
	int y = tileCoords / m_dimensions.x;
	int x = tileCoords % m_dimensions.x;
	return IntVec2(x, y);
}

//------------------------------------------------------------------------------
void Map::Render() const
{
	std::vector<Vertex> tileVerts;

	int totalTiles = static_cast<int>(m_tiles.size());
	tileVerts.reserve(totalTiles * 6);

	for (int tileIndex = 0; tileIndex < totalTiles; ++tileIndex)
	{
		Tile const& tile = m_tiles[tileIndex];

		float minX = static_cast<float>(tile.m_tileCoords.x);
		float minY = static_cast<float>(tile.m_tileCoords.y);
		float maxX = minX + 1.0f;
		float maxY = minY + 1.0f;

		AABB2 box = AABB2(minX, minY, maxX, maxY);
		Rgba8 tileColor;
		if (tile.m_type == GRASS)
		{
			tileColor = GRASS_COLOR;
		}
		else
		{
			tileColor = STONE_COLOR;
		}

		AddVertsForAABB2D(tileVerts, box, tileColor);
	}

	g_engine->m_render->DrawVertexArray(static_cast<int>(tileVerts.size()), tileVerts.data());

}

void Map::BuildMapTiles()
{
	int totalTiles = m_dimensions.x * m_dimensions.y;
	m_tiles.resize(totalTiles);

	GrassTileSetup();
	StoneTileSetup();
	OutEdgeStoneSetup();
	BarrierTileSetup();
}

//------------------------------------------------------------------------------
// Tile Setups
//------------------------------------------------------------------------------
void Map::BarrierTileSetup()
{
	m_tiles[GetTileIndexForTileCoords(IntVec2(2, 4))].m_type = STONE;
	m_tiles[GetTileIndexForTileCoords(IntVec2(3, 4))].m_type = STONE;
	m_tiles[GetTileIndexForTileCoords(IntVec2(4, 4))].m_type = STONE;
	m_tiles[GetTileIndexForTileCoords(IntVec2(4, 3))].m_type = STONE;
	m_tiles[GetTileIndexForTileCoords(IntVec2(4, 2))].m_type = STONE;
}

//------------------------------------------------------------------------------
void Map::OutEdgeStoneSetup()
{
	for (int tileY = 0; tileY < m_dimensions.y; ++tileY)
	{
		for (int tileX = 0; tileX < m_dimensions.x; ++tileX)
		{
			bool isEdge = (tileX == 0 || tileX == m_dimensions.x - 1 ||
				tileY == 0 || tileY == m_dimensions.y - 1);

			if (isEdge)
			{
				int tileIndex = GetTileIndexForTileCoords(IntVec2(tileX, tileY));
				m_tiles[tileIndex].m_type = STONE;
			}
		}
	}
}

//------------------------------------------------------------------------------
void Map::StoneTileSetup()
{
	for (int tileY = 6; tileY < m_dimensions.y - 1; ++tileY)
	{
		for (int tileX = 6; tileX < m_dimensions.x - 1; ++tileX)
		{
			if (g_rng.RollRandomFloatZeroToOne() < 0.1f)
			{
				int tileIndex = GetTileIndexForTileCoords(IntVec2(tileX, tileY));
				m_tiles[tileIndex].m_type = STONE;
			}
		}
	}
}

//------------------------------------------------------------------------------
void Map::GrassTileSetup()
{
	for (int tileY = 0; tileY < m_dimensions.y; ++tileY)
	{
		for (int tileX = 0; tileX < m_dimensions.x; ++tileX)
		{
			int tileIndex = GetTileIndexForTileCoords(IntVec2(tileX, tileY));
			m_tiles[tileIndex].m_tileCoords = IntVec2(tileX, tileY);
			m_tiles[tileIndex].m_type = GRASS;
		}
	}
}
