#include "Game/Game.hpp"
#include "Game/CubicBezierCurve2D.hpp"
#include "Game/CubicHermiteCurve2D.hpp"
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

	RandomNumberGenerator* g_rng;

	void Startup() override;
	void Shutdown() override;
	void Update( float deltaSeconds ) override;
	void UpdateKeyboardInput();
	void Render() const override;
	void RenderEasingGraph( int numSamples = 64, EasingFunction method = EASING_CustomFunkyEasingFunction ) const;
	void RenderBezierCurve( int numSamples = 64 ) const;
	void RenderHermiteCurve( int numSamples = 64 ) const;

private:
	EasingFunction m_currentEasingFunction = EASING_Hesitate5;
	int m_numSamples = 64;
	CubicBezierCurve2D m_bezierCurve;
	std::vector<CubicHermiteCurve2D> m_hermiteCurves;
	int m_selectedPointIndex = -1;
	float m_controlPointRadius = 2.0f;
	int m_hermiteCurvePoints = 10;

	float GetEasedValue( EasingFunction method, float t ) const;
	void CreateHermiteCurve();
};