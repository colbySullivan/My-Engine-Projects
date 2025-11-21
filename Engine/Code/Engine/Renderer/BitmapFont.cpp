#include "Engine/Renderer/BitmapFont.hpp"

//-----------------------------------------------------------------------------------------------
BitmapFont::BitmapFont( char const* fontFilePathNameWithNoExtension, Texture& fontTexture )
	:	m_fontFilePathNameWithNoExtension( fontFilePathNameWithNoExtension )
	,	m_fontGlyphsSpriteSheet( fontTexture, IntVec2(16, 16) )
{
	
}

//-----------------------------------------------------------------------------------------------
Texture& BitmapFont::GetTexture()
{
	return m_fontGlyphsSpriteSheet.GetTexture();
}

//-----------------------------------------------------------------------------------------------
void BitmapFont::AddVertsForText2D( std::vector<Vertex>& vertexArray, Vec2 textMins, float cellHeight, std::string const& text, Rgba8 tint, float cellAspectScale )
{
	float cellWidth = cellHeight * cellAspectScale;
	Vec2 currentMins = textMins;

	for (int charIndex = 0; charIndex < text.size() ; ++charIndex)
	{
		SpriteDefinition const& sprite = m_fontGlyphsSpriteSheet.GetSpriteDef( text[charIndex] );
		Vec2 uvMins, uvMaxs;
		sprite.GetUVs( uvMins, uvMaxs );

		float minX = currentMins.x;
		float minY = currentMins.y;
		float maxX = currentMins.x + cellWidth;
		float maxY = currentMins.y + cellHeight;

		vertexArray.push_back( Vertex( Vec3( minX, minY, 0.f ), tint, Vec2( uvMins.x, uvMins.y ) ) );
		vertexArray.push_back( Vertex( Vec3( maxX, minY, 0.f ), tint, Vec2( uvMaxs.x, uvMins.y ) ) );
		vertexArray.push_back( Vertex( Vec3( maxX, maxY, 0.f ), tint, Vec2( uvMaxs.x, uvMaxs.y ) ) );
		vertexArray.push_back( Vertex( Vec3( minX, minY, 0.f ), tint, Vec2( uvMins.x, uvMins.y ) ) );
		vertexArray.push_back( Vertex( Vec3( maxX, maxY, 0.f ), tint, Vec2( uvMaxs.x, uvMaxs.y ) ) );
		vertexArray.push_back( Vertex( Vec3( minX, maxY, 0.f ), tint, Vec2( uvMins.x, uvMaxs.y ) ) );

		currentMins.x += cellWidth;
	}
}

//-----------------------------------------------------------------------------------------------
float BitmapFont::GetTextWidth( float cellHeight, std::string const& text, float cellAspectScale )
{
	float cellWidth = cellHeight * cellAspectScale;
	float gapWidth = cellWidth * cellAspectScale;
	float numCells = static_cast<float>( text.length() );
	float numGaps = numCells - 1.f;
	if ( numGaps < 0.f )
		numGaps = 0.f;

	float totalWidth = ( numCells * cellWidth ) + ( numGaps * gapWidth );
	return totalWidth;
}

//-----------------------------------------------------------------------------------------------
float BitmapFont::GetGlyphAspect( [[maybe_unused]] int glyphUnicode ) const
{
	return m_fontDefaultAspect;
}

