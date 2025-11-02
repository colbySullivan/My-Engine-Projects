#pragma once
#include "Game/Game.hpp"
#include "Game/TestShape.hpp"
#include "Game/TestShape.hpp"
#include "Game/TestShapeLine.hpp"



//-----------------------------------------------------------------------------------------------
class GameRaycastVsDiscs : public Game {

public:
	GameRaycastVsDiscs( App* app );
	~GameRaycastVsDiscs();

	void Startup() override;
	void Update( float deltaSeconds ) override;
	void Render() const override;

private:
	void AddShapeVerts();
	void RenderShapes() const;
	std::vector<TestShape*> m_testShapes;

};
