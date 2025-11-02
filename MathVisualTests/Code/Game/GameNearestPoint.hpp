#pragma once
#include "Game/Game.hpp"
#include "Game/TestShape.hpp"
#include "Game/TestShapeLine.hpp"
#include "Game/TestShapeTriangle.hpp"
#include "Game/TestShapeOBB2.hpp"
#include "Game/TestShapeDisc.hpp"
#include "Game/TestShapeAABB2.hpp"

//-----------------------------------------------------------------------------------------------
class GameNearestPoint : public Game {

public:
	GameNearestPoint(App* app);
	~GameNearestPoint();

	void Startup() override;
	void Update( float deltaSeconds ) override;
	void Render() const override;

private:
	void AddShapeVerts();
	
	void UpdatePointPosition( float deltaSeconds );
	void RenderShapes() const;
	std::vector<Vertex> m_pointVerts;
	std::vector<TestShape*> m_testShapes;
	//TestShapeLine*	m_line;
	Vec2 m_pointPos = Vec2( 10.f, 60.f );
	float m_speed = 50.f;
};
