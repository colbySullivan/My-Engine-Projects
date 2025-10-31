#pragma once
#include "Game/Game.hpp"
#include "Game/TestShape.hpp"
#include "Game/TestShapeLine.hpp"

//-----------------------------------------------------------------------------------------------
class GameNearestPoint : public Game {

public:
	GameNearestPoint(App* app);
	~GameNearestPoint();

	void Update( float deltaSeconds ) override;
	void Render() const override;

private:
	void AddShapeVerts();
	TestShapeLine*	m_line;
};
