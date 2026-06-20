#pragma once
#include "Engine/Core/UIButton2D.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Game/ItemDefinitions.hpp"
#include <string>
#include <vector>

class ShopItemCard
{
public:
	ShopItemCard( const ItemDefinitions* item, Vec2 centerPos, float sizeX, float sizeY );
	~ShopItemCard();

	void Update( const Vec2& mousePos, bool isMouseLeftJustPressed );
	void Render() const;

private:
	void BuildStatLines();

	UIButton2D* m_button = nullptr;
	const ItemDefinitions* m_item = nullptr;
	Texture* m_icon = nullptr;
	BitmapFont* m_font = nullptr;
	AABB2 m_bounds;
	std::vector<std::string> m_statLines;
};