#pragma once
#include "Game/Game.hpp"
#include "TestShape.hpp"
#include "TestShapeDisc.hpp"

class Game2DPachinkoMachine : public Game
{
public:
	Game2DPachinkoMachine( App* app );
	~Game2DPachinkoMachine();

	void Startup() override;
	void Shutdown() override;
	void Update( float deltaSeconds ) override;
	void Render() const override;

private:
	std::vector<TestShape*> m_testShapes;
	std::vector<TestShapeDisc*> m_balls;
	Vec2					m_pointPos;
	float					m_elasticity = 0.9f;
	float					m_wallElasticity = 0.9f;
	Vec2					m_tailPos;
	Vec2					m_tipPos;

	void AddShapes();

	void RenderShapes() const;
	void RenderMouseOutline() const;
	void RenderVelocityLine() const;

	void UpdateMousePosition();
	void UpdateSpawnBalls();
	void UpdateBounceBallsEachOther();
	void UpdateGravity( float deltaSeconds );
	void UpdateVelocityPoints();
	void UpdateBallsWalls();
	void UpdateBounceBallsBumpers();
};