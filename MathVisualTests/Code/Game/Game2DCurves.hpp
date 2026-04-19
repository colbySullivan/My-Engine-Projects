#pragma once

#include "Game/Game.hpp"
#include "Engine/Math/MathUtils.hpp"

class Window;

enum EasingFunction
{
	EASING_SmoothStart2,
	EASING_SmoothStart3,
	EASING_SmoothStart4,
	EASING_SmoothStart5,
	EASING_SmoothStart6,
	EASING_SmoothStop2,
	EASING_SmoothStop3,
	EASING_SmoothStop4,
	EASING_SmoothStop5,
	EASING_SmoothStop6,
	EASING_SmoothStep3,
	EASING_SmoothStep5,
	EASING_Hesitate3,
	EASING_Hesitate5,
	EASING_CustomFunkyEasingFunction,
	numEasingFunctions
};

//-----------------------------------------------------------------------------------------------
class Game2DCurves : public Game {

public:
	Game2DCurves( App* app );
	~Game2DCurves();

	void Startup() override;
	void Shutdown() override;
	void Update( float deltaSeconds ) override;
	void UpdateKeyboardInput();
	void Render() const override;
	void RenderEasingGraph( int numSamples, EasingFunction method ) const;
	float GetEasedValue(EasingFunction method, float t) const;
private:
	EasingFunction m_currentEasingFunction = EASING_Hesitate5;
};
