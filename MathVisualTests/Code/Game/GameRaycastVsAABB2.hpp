#pragma once
#include "Game/Game.hpp"
#include "Game/TestShape.hpp"
#include "Game/TestShape.hpp"
#include "Game/TestShapeLine.hpp"
#include "Game/TestShapeAABB2.hpp"
#include "Game/TestShapeDisc.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

//class Window;

//-----------------------------------------------------------------------------------------------
class GameRaycastVsAABB2 : public Game {

public:
	GameRaycastVsAABB2( App* app );
	~GameRaycastVsAABB2();

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
	std::vector<TestShapeAABB2*> m_testShapes;
	std::vector<Vertex> m_lineVerts;
	TestShapeAABB2*		m_lastClosestShape = nullptr;
	Vec2				m_tailPos;
	Vec2				m_tipPos;
	int					m_numberOfLines = 2;
	std::vector<RaycastResult2D> m_raycastResults;
};
