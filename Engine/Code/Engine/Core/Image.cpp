#define STB_IMAGE_IMPLEMENTATION // Exactly one .CPP (this Image.cpp) should #define this before #including stb_image.h
#include "ThirdParty/stb/stb_image.h"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

//-----------------------------------------------------------------------------------------------
Image::Image( char const* imageFilePath )
	:m_imageFilePath( imageFilePath )
{
	int numComponents = 0;

	stbi_set_flip_vertically_on_load( 1 );

	unsigned char* imageDataByes = stbi_load( imageFilePath, &m_dimensions.x, &m_dimensions.y, &numComponents, 0 );
	GUARANTEE_OR_DIE( imageDataByes != nullptr, Stringf( "Failed to load image %s", imageFilePath ) );
	GUARANTEE_OR_DIE( numComponents == 3 || numComponents == 4, Stringf( "Can't load image from file %s; has %i components; we only support 3 or 4", imageFilePath, numComponents ) );

	int numTexels = m_dimensions.x * m_dimensions.y;
	m_texelRgba8Data.resize( numTexels );

	for ( int texelIndex = 0; texelIndex < numTexels; ++texelIndex )
	{
		Rgba8& texel = m_texelRgba8Data[texelIndex];
		texel.r = imageDataByes[( texelIndex * numComponents ) + 0];
		texel.g = imageDataByes[( texelIndex * numComponents ) + 1];
		texel.b = imageDataByes[( texelIndex * numComponents ) + 2];
		texel.a = ( numComponents == 3 ) ? 255 : imageDataByes[( texelIndex * numComponents ) + 3];
	}

	stbi_image_free( imageDataByes );
}

//-----------------------------------------------------------------------------------------------
Image::Image( IntVec2 size, Rgba8 color )
	: m_dimensions( size )
{
	int numTexels = m_dimensions.x * m_dimensions.y;
	m_texelRgba8Data.resize( numTexels, color );
}

//-----------------------------------------------------------------------------------------------
IntVec2 Image::GetDimensions() const
{
	return m_dimensions;
}

//-----------------------------------------------------------------------------------------------
const std::string& Image::GetImageFilePath() const
{
	return m_imageFilePath;
}

//-----------------------------------------------------------------------------------------------
const void* Image::GetRawData() const
{
	return &m_texelRgba8Data;
}
