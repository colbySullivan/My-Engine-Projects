#pragma once
#include "Engine/Math/AABB2.hpp"
#include <string>
#include "Rgba8.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

class UIButton2D
{
public:
	AABB2 m_bounds;
	std::string m_buttonText;
	Rgba8 m_baseColor;
	Rgba8 m_hoveredColor;
	bool m_isCurrentlyHovered = false;
	BitmapFont* m_font;

public:
	UIButton2D( const Vec2& min, const Vec2& max, const std::string& buttonText, Rgba8 baseColor, Rgba8 hoveredColor=Rgba8(255,255,255,100) );
	UIButton2D( const Vec2& centerPosition, const float sizeX, const float sizeY, const std::string& buttonText, Rgba8 baseColor, Rgba8 hoveredColor=Rgba8(255,255,255,100) );
	
	//void Update();
	void Render() const;

	void UpdateHoverState( const Vec2& mousePos );
	bool IsHovered() const;
	void isHovered( Vec2 mousePos );
};