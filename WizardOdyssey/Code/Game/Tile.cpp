#include "Game/Tile.hpp"
#include "Game/TileDefinitions.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

//-----------------------------------------------------------------------------------------------
Tile::Tile()
{

}

//-----------------------------------------------------------------------------------------------
Tile::Tile( IntVec2 tileCoords, std::string type )
	: m_tileCoords( tileCoords )
	, m_type( type )
{

}

//-----------------------------------------------------------------------------------------------
Tile::~Tile()
{

}

//-----------------------------------------------------------------------------------------------
bool Tile::IsSolid() const
{
	if ( m_type == "INVALID_TILE_TYPES" )
		return false;

	return TileDefinition::s_definitions[m_type].m_isSolid;
}

//-----------------------------------------------------------------------------------------------
std::string const& Tile::GetDefinition() const
{
	return m_type;
}
