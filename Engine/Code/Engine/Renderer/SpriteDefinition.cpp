#include "Engine/Renderer/SpriteDefinition.hpp"

//-----------------------------------------------------------------------------------------------
void SpriteDefinition::GetUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs ) const
{
	out_uvAtMins = m_uvAtMins;
	out_uvAtMaxs = m_uvAtMaxs;
}

//-----------------------------------------------------------------------------------------------
AABB2 SpriteDefinition::GetUVs() const
{
	return AABB2( m_uvAtMins, m_uvAtMaxs );
}

//-----------------------------------------------------------------------------------------------
SpriteSheet const& SpriteDefinition::GetSpriteSheet() const
{
	return *m_spriteSheet;
}

//-----------------------------------------------------------------------------------------------
Texture& SpriteDefinition::GetTexture() const
{
	return m_spriteSheet->GetTexture();
}

//-----------------------------------------------------------------------------------------------
float SpriteDefinition::GetAspect() const
{
	float uvWidth = m_uvAtMaxs.x - m_uvAtMins.x;
	float uvHeight = m_uvAtMaxs.y - m_uvAtMins.y;

	Texture& texture = m_spriteSheet->GetTexture();
	IntVec2 textureDimensions = texture.GetDimensions();

	float spritePixelWidth = uvWidth * static_cast< float >( textureDimensions.x );
	float spritePixelHeight = uvHeight * static_cast< float >( textureDimensions.y );

	if ( spritePixelHeight != 0.f )
	{
		return spritePixelWidth / spritePixelHeight;
	}

	return 1.f;
}