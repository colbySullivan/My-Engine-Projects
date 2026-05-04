#pragma once
#include "Game/Game.hpp"
#include "TestShape.hpp"

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

	void AddShapes();
	void RenderShapes() const;
	void UpdateMousePosition();
	Vec2 m_pointPos;
};