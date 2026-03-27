#pragma once

#include "Game/Game.hpp"
#include "Game/Player.hpp"


//-----------------------------------------------------------------------------------------------
class TestShape3D : public Game {

public:
	TestShape3D( App* app );
	~TestShape3D();

	void Startup() override;
	void Shutdown() override;
	void Update( float deltaSeconds ) override;
	void Render() const override;

private:
	Player* m_player = nullptr;
};
