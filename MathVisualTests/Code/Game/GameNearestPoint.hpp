#pragma once
#include "Game/Game.hpp"

class GameNearestPoint : public Game {

public:
	GameNearestPoint(App* app);

	virtual void Update( float deltaTime );
	virtual void Render();

private:
	void AddShapeVerts();
	std::vector<Vertex> m_lineVerts;
};