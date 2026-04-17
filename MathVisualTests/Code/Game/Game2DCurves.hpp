#pragma once

#include "Game/Game.hpp"

class Window;

//-----------------------------------------------------------------------------------------------
class Game2DCurves : public Game {

public:
	Game2DCurves( App* app );
	~Game2DCurves();

	void Startup() override;
	void Shutdown() override;
	void Update( float deltaSeconds ) override;
	void Render() const override;

private:

};
