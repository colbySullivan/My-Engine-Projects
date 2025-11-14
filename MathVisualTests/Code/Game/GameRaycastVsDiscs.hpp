#pragma once
#include "Game/Game.hpp"
#include "Game/TestShape.hpp"
#include "Game/TestShape.hpp"
#include "Game/TestShapeLine.hpp"
#include "Game/TestShapeDisc.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

//class Window;

//-----------------------------------------------------------------------------------------------
class GameRaycastVsDiscs : public Game {

public:
	GameRaycastVsDiscs( App* app );
	~GameRaycastVsDiscs();

	void Startup() override;
	void Update( float deltaSeconds ) override;
	void Render() const override;

	RandomNumberGenerator* g_rng;
private:
	void AddShapeVerts();
	void RenderShapes() const;
	void UpdateLine();
	void UpdateKeyboardPoints();

	Vec2 GetRandomPosition( float minX, float maxX, float minY, float maxY );
	std::vector<TestShape*> m_testShapes;
	std::vector<Vertex> m_lineVerts;
	Vec2				m_tailPos;
	Vec2				m_tipPos;
	//void*				m_windowHandle = nullptr;
};
