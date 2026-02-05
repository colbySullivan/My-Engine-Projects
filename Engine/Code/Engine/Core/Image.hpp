#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <vector>
#include <string>

class Image
{
	friend class Renderer;

public:
	Image() = default;
	~Image() = default;
	Image( const char* imageFilePath );
	Image( IntVec2 size, Rgba8 color );

	IntVec2 GetDimensions() const;
	const std::string & GetImageFilePath() const;
	const void* GetRawData() const;

private:
	std::string m_imageFilePath;
	IntVec2 m_dimensions;
	std::vector<Rgba8> m_texelRgba8Data;

};