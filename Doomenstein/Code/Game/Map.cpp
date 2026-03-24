#include "Game/Map.hpp"

//-----------------------------------------------------------------------------------------------
Map::Map( Game* game, const MapDefinition* definition )
	: m_game( game )
	, m_texture( definition->m_spriteSheetTexture )
	, m_shader( definition->m_shader )
{
}

//-----------------------------------------------------------------------------------------------
Map::~Map()
{

}
