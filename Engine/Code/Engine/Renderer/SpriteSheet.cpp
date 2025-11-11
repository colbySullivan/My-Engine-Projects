#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"

//-----------------------------------------------------------------------------------------------
SpriteSheet::SpriteSheet( Texture& texture, IntVec2 const& simpleGridLayout )
	: m_texture( texture )
{
	int numSprites = simpleGridLayout.x * simpleGridLayout.y;
	m_spriteDefs.resize( numSprites );

	for ( int spriteY = 0; spriteY < simpleGridLayout.y; ++spriteY )
	{
		for ( int spriteX = 0; spriteX < simpleGridLayout.x; ++spriteX )
		{
			int spriteIndex = spriteX + ( simpleGridLayout.x * spriteY );
			Vec2 uvAtMins( static_cast< float >( spriteX ) / simpleGridLayout.x, static_cast< float >( spriteY ) / simpleGridLayout.y );
			Vec2 uvAtMaxs( static_cast< float >( spriteX + 1 ) / simpleGridLayout.x, static_cast< float >( spriteY + 1 ) / simpleGridLayout.y );
			SpriteSheet const& spriteSheet = *this;

			m_spriteDefs[spriteIndex].m_spriteSheet = &spriteSheet;
			m_spriteDefs[spriteIndex].m_uvAtMins = uvAtMins;
			m_spriteDefs[spriteIndex].m_uvAtMaxs = uvAtMaxs;
			m_spriteDefs[spriteIndex].m_spriteIndex = spriteIndex;
		}
	}
}

//-----------------------------------------------------------------------------------------------
Texture& SpriteSheet::GetTexture() const
{
	return m_texture;
}

//-----------------------------------------------------------------------------------------------
int SpriteSheet::GetNumSprites() const
{
	return static_cast< int >( m_spriteDefs.size() );
}

//-----------------------------------------------------------------------------------------------
SpriteDefinition const& SpriteSheet::GetSpriteDef( int spriteIndex ) const
{
	return m_spriteDefs[spriteIndex];
}

//-----------------------------------------------------------------------------------------------
void SpriteSheet::GetSpriteUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex ) const
{
	m_spriteDefs[spriteIndex].GetUVs( out_uvAtMins, out_uvAtMaxs );
}

//-----------------------------------------------------------------------------------------------
AABB2 SpriteSheet::GetSpriteUVs( int spriteIndex ) const
{
	return m_spriteDefs[spriteIndex].GetUVs();
}