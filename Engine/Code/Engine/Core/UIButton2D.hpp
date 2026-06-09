#pragma once
#include "Engine/Math/AABB2.hpp"
#include <string>
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

class UIButton2D
{
public:
	UIButton2D( const Vec2& min, const Vec2& max, const std::string& buttonText, const std::string& onClickEvent, Rgba8 baseColor, Rgba8 hoveredColor=Rgba8(255,255,255,100) );
	UIButton2D( const Vec2& centerPosition, const float sizeX, const float sizeY, const std::string& buttonText, const std::string& onClickEvent, Rgba8 baseColor, Rgba8 hoveredColor=Rgba8(255,255,255,100) );
	
	void Update( const Vec2& mousePos, bool isMouseLeftJustPressed );
	void Render() const;

	void UpdateHoverState( const Vec2& mousePos );
	bool IsHovered() const;
	void isHovered( Vec2 mousePos );

private:
	AABB2       m_bounds;
	std::string m_buttonText;
	Rgba8       m_baseColor;
	Rgba8       m_hoveredColor;
	BitmapFont* m_font;

	bool        m_isCurrentlyHovered = false;

	std::string m_onClickEvent;
	EventArgs   m_eventArgs;
};
