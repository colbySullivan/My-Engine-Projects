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
	void UpdateLine();
	void UpdateKeyboardPoints();

	std::vector<Vertex> m_lineVerts;
	Vec2				m_tailPos;
	Vec2				m_tipPos;
};
