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

//------------------------------------------------------------------------------
void BitmapFont::AddVertsForTextInBox2D(std::vector<Vertex>& verts, std::string const& text, AABB2 const& box, 
	float cellHeight, Rgba8 tint, float cellAspectScale, Vec2 alignment, TextBoxMode mode, int maxGlyphsToDraw)
{
	Vec2 boxDimensions = box.GetDimensions();
	float textWidth = GetTextWidth(cellHeight, text, cellAspectScale);

	if (mode == SHRINK_TO_FIT)
	{
		if (textWidth > boxDimensions.x)
		{
			float scale = boxDimensions.x / textWidth;
			cellHeight *= scale;
			textWidth = GetTextWidth(cellHeight, text, cellAspectScale);
		}
	}

	Vec2 startPos = box.m_mins;
	Vec2 offset;
	offset.x = (boxDimensions.x - textWidth) * alignment.x;
	offset.y = (boxDimensions.y - cellHeight) * alignment.y;
	startPos = box.m_mins + offset;

	AddVertsForText2D(verts, startPos, cellHeight, text, tint, cellAspectScale);
}

//-----------------------------------------------------------------------------------------------
float BitmapFont::GetTextWidth( float cellHeight, std::string const& text, float cellAspectScale )
{
	float cellWidth = cellHeight * cellAspectScale;
	float numCells = static_cast<float>( text.length() );
	float totalWidth = ( numCells * cellWidth );
	return totalWidth;
}

//-----------------------------------------------------------------------------------------------
float BitmapFont::GetGlyphAspect( [[maybe_unused]] int glyphUnicode ) const
{
	return m_fontDefaultAspect;
}

