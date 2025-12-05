#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/StringUtils.hpp"

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
	Strings splitStrings = SplitStringOnDelimiter(text, '\n');
	Vec2 boxDimensions = box.GetDimensions();

	float textWidth = 0.f;
	for (int stringIndex = 0; stringIndex < splitStrings.size(); ++stringIndex)
	{
		float lineWidth = GetTextWidth(cellHeight, splitStrings[stringIndex], cellAspectScale);
		if (lineWidth > textWidth)
		{
			textWidth = lineWidth;
		}
	}

	float paragraphHeight = cellHeight * static_cast<float>( splitStrings.size() );

	if (mode == SHRINK_TO_FIT)
	{
		if (textWidth > boxDimensions.x)
		{
			float scale = boxDimensions.x / textWidth;
			cellHeight *= scale;
			textWidth *= scale;
		}

		if (paragraphHeight > boxDimensions.y)
		{
			float scale = boxDimensions.y / paragraphHeight;
			cellHeight *= scale;
			textWidth *= scale;
			paragraphHeight = boxDimensions.y;
		}
	}

	Vec2 offset;
	offset.x = (boxDimensions.x - textWidth) * alignment.x;
	offset.y = (boxDimensions.y - paragraphHeight) * alignment.y;
	Vec2 startPos = box.m_mins + offset;

	int glyphsDrawn = 0;
	int splitStringSize = static_cast<int>( splitStrings.size() ) - 1;
	for (int stringIndex = 0; stringIndex <= splitStringSize; ++stringIndex)
	{
		Vec2 linePos = startPos;
		linePos.y += cellHeight * ( splitStringSize - stringIndex );

		if ( glyphsDrawn <= maxGlyphsToDraw )
		{
			int lengthNextString = static_cast<int>( splitStrings[stringIndex].length() );
			if ( lengthNextString + glyphsDrawn < maxGlyphsToDraw )
			{
				float lineWidth = GetTextWidth(cellHeight, splitStrings[stringIndex], cellAspectScale);
				linePos.x = box.m_mins.x + (boxDimensions.x - lineWidth) * alignment.x;
				AddVertsForText2D( verts, linePos, cellHeight, splitStrings[stringIndex], tint, cellAspectScale );
				glyphsDrawn += static_cast<int>( splitStrings[stringIndex].length() );
			}
			else
			{
				break;
			}
		}
	}
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

