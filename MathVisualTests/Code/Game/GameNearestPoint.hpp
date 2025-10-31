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
	
	void UpdatePointPosition( float deltaTime );
	std::vector<Vertex> m_pointVerts;
	TestShapeLine*	m_line;
	Vec2 m_pointPos = Vec2( 10.f, 60.f );
	float m_speed = 50.f;
};
