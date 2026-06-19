#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/UIButton2D.hpp"

class GameButtonDefinitions
{
public:
	GameButtonDefinitions() = default;
	~GameButtonDefinitions() = default;

	static void InitializeButtonDefs();
	static void LoadButtonDefsFromFile( char const* filePath );

	int	gameState = 0;
	std::string name = "";
	UIButton2D* buttonRef = nullptr;
	Vec2 positionFraction; 
	float sizeX;
	float sizeY; 
	std::string buttonText;
	std::string onClickEvent; 
	std::string argName;
	std::string argValue; 
	std::string defaultTexture; 
	std::string hoveredTexture; 


	Rgba8 baseColor; 
	Rgba8 hoveredColor;

	static std::vector<GameButtonDefinitions> s_definitions;
};