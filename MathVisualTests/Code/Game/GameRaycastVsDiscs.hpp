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
	void Shutdown() override;
	void Update( float deltaSeconds ) override;
	void Render() const override;

	RandomNumberGenerator* g_rng;
private:
	void AddShapeVerts();
	void RenderShapes() const;
	void UpdateLine();
	void UpdateKeyboardPoints( float deltaSeconds );
	void UpdateCheckDiscsRaycast();

	Vec2 GetRandomPosition( float minX, float maxX, float minY, float maxY );
	std::vector<TestShapeDisc*> m_testShapes;
	std::vector<Vertex> m_lineVerts;
	TestShapeDisc*		m_lastClosestShape = nullptr;
	Vec2				m_tailPos;
	Vec2				m_tipPos;
	int					m_numberOfDiscs = 10;
	std::vector<RaycastResult2D> m_raycastResults;
};
