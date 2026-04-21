#include "Game/Game2DCurves.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/EngineCommon.hpp"

//------------------------------------------------------------------------------
typedef float ( *EasingFuncPtr )( float t );

//------------------------------------------------------------------------------
static const EasingFuncPtr easingLookup[numEasingFunctions] =
{
	SmoothStart2,
	SmoothStart3,
	SmoothStart4,
	SmoothStart5,
	SmoothStart6,
	SmoothStop2,
	SmoothStop3,
	SmoothStop4,
	SmoothStop5,
	SmoothStop6,
	SmoothStep3,
	SmoothStep5,
	Hesitate3,
	Hesitate5,
	CustomFunkyEasingFunction
};

//------------------------------------------------------------------------------
static const char* easingFunctionNames[numEasingFunctions] =
{
	"SmoothStart2",
	"SmoothStart3",
	"SmoothStart4",
	"SmoothStart5",
	"SmoothStart6",
	"SmoothStop2",
	"SmoothStop3",
	"SmoothStop4",
	"SmoothStop5",
	"SmoothStop6",
	"SmoothStep3",
	"SmoothStep5",
	"Hesitate3",
	"Hesitate5",
	"FunkyFresh"
};

//-----------------------------------------------------------------------------------------------
Game2DCurves::Game2DCurves( App* app )
	: Game( app )
	, m_bezierCurve(
		Vec2( 130.f + g_rng->RollRandomFloatInRange( 20.f, 40.f ), 70.f + g_rng->RollRandomFloatInRange( 0.f, 20.f ) ),
		Vec2( 140.f + g_rng->RollRandomFloatInRange( 20.f, 40.f ), 60.f + g_rng->RollRandomFloatInRange( 10.f, 30.f ) ),
		Vec2( 160.f + g_rng->RollRandomFloatInRange( 20.f, 40.f ), 40.f + g_rng->RollRandomFloatInRange( 20.f, 40.f ) ),
		Vec2( 150.f + g_rng->RollRandomFloatInRange( 20.f, 30.f ), 80.f + g_rng->RollRandomFloatInRange( 0.f, 20.f ) )
	)
{
}

Game2DCurves::~Game2DCurves()
{
	Shutdown();
}

void Game2DCurves::Startup()
{
	Game::Startup();
	CreateHermiteCurve();
}

void Game2DCurves::Shutdown()
{

}

void Game2DCurves::Update( float deltaSeconds )
{
	UpdateCameras( deltaSeconds );
	UpdateKeyboardInput();
	Game::UpdateKeyboardInput();
}

//------------------------------------------------------------------------------
void Game2DCurves::UpdateKeyboardInput()
{
	if ( g_engine->m_input->WasKeyJustPressed( 'W' ) )
	{
		m_currentEasingFunction = static_cast< EasingFunction >( ( m_currentEasingFunction + 1 ) % numEasingFunctions );
	}
	
	if ( g_engine->m_input->WasKeyJustPressed( 'E' ) )
	{
		m_currentEasingFunction = static_cast< EasingFunction >( ( m_currentEasingFunction - 1 + numEasingFunctions ) % numEasingFunctions );
	}

	if ( g_engine->m_input->WasKeyJustPressed( 'N' ) )
	{
		m_numSamples -= 8;
	}

	if ( g_engine->m_input->WasKeyJustPressed( 'M' ) )
	{
		m_numSamples += 8;
	}
}

void Game2DCurves::Render() const
{
	g_engine->m_render->BindTexture( nullptr );

	Rgba8 backgroundColor = Rgba8( static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 255.f ) );

	g_engine->m_render->ClearScreen( backgroundColor );

	g_engine->m_render->BeginCamera( *m_worldCamera );

	RenderEasingGraph( m_numSamples, m_currentEasingFunction );

	RenderBezierCurve( m_numSamples );

	RenderHermiteCurve( m_numSamples );

	Game::RenderGameText( GAMEMODE_2D_CURVES );

	g_engine->m_render->EndCamera( *m_worldCamera );
}

//------------------------------------------------------------------------------
void Game2DCurves::RenderEasingGraph( int numSamples, EasingFunction method ) const
{
	float graphTime = fmodf( static_cast< float >( g_engine->m_systemClock->GetTotalSeconds() ), 1.f );

	float padding = 10.f;
	float sideLength = 30.f;
	Vec2 graphMin = Vec2( padding, 100.f - padding - sideLength );
	Vec2 graphMax = Vec2( padding + sideLength, 100.f - padding );
	float graphWidth = graphMax.x - graphMin.x;
	float graphHeight = graphMax.y - graphMin.y;

	g_engine->m_render->BindTexture( &g_testFont->GetTexture() );
	std::vector<Vertex> titleVerts;
	std::string currentMethodName = easingFunctionNames[method];
	g_testFont->AddVertsForTextInBox2D( titleVerts, currentMethodName, AABB2( Vec2( graphMin.x - 5.f, graphMin.y - 5.f ), Vec2( graphMax.x + 5.f, graphMin.y + 5.f ) ), 2.f, Rgba8::WHITE, 1.f, Vec2( 0.5f, 0.f ) );
	g_engine->m_render->DrawVertexArray( static_cast< int >( titleVerts.size() ), titleVerts.data() );

	g_engine->m_render->BindTexture( nullptr );

	std::vector<Vertex> axisVerts;
	AddVertsForAABB2D( axisVerts, AABB2( graphMin, graphMax ), Rgba8( 120, 120, 120, 120 ) );
	g_engine->m_render->DrawVertexArray( static_cast< int >( axisVerts.size() ), axisVerts.data() );

	std::vector<Vertex> curveVerts;

	for ( int i = 0; i < numSamples - 1; ++i )
	{
		float t0 = static_cast< float >( i ) / static_cast< float >( numSamples - 1 );
		float t1 = static_cast< float >( i + 1 ) / static_cast< float >( numSamples - 1 );

		float easedValue0 = GetEasedValue( method, t0 );
		float easedValue1 = GetEasedValue( method, t1 );

		Vec2 point0 = Vec2( graphMin.x + t0 * graphWidth, graphMin.y + easedValue0 * graphHeight );
		Vec2 point1 = Vec2( graphMin.x + t1 * graphWidth, graphMin.y + easedValue1 * graphHeight );

		AddVertsForLineSegment2D( curveVerts, point0, point1, Vec2( .25f, .25f ), Rgba8::GREEN );
	}

	g_engine->m_render->DrawVertexArray( static_cast< int >( curveVerts.size() ), curveVerts.data() );

	float easedValue = GetEasedValue( method, graphTime );
	Vec2 currentPoint = Vec2( graphMin.x + graphTime * graphWidth, graphMin.y + easedValue * graphHeight );
	std::vector<Vertex> pointVerts;
	AddVertsForDisc2D( pointVerts, currentPoint, 0.5f, Rgba8::WHITE );
	g_engine->m_render->DrawVertexArray( static_cast< int >( pointVerts.size() ), pointVerts.data() );
}

//------------------------------------------------------------------------------
float Game2DCurves::GetEasedValue( EasingFunction method, float t ) const
{
	return easingLookup[method]( t );
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
void Game2DCurves::CreateHermiteCurve()
{
	std::vector<Vec2> points( m_hermiteCurvePoints );
	for ( int i = 0; i < m_hermiteCurvePoints; ++i )
	{
		float xPos = static_cast< float >( i ) * 20.f + g_rng->RollRandomFloatInRange( 5.f, 20.f );
		float yPos = g_rng->RollRandomFloatInRange( 10.f, 50.f );
		points[i] = Vec2( xPos, yPos );
	}
	for ( int i = 0; i < m_hermiteCurvePoints; ++i )
	{
		Vec2 startPos = points[i];
		Vec2 endPos = points[( i + 1 ) % m_hermiteCurvePoints];
		Vec2 startVelocity = ( points[( i + 1 ) % m_hermiteCurvePoints] - points[( i - 1 + m_hermiteCurvePoints ) % m_hermiteCurvePoints] ) * 0.5f;
		Vec2 endVelocity = ( points[( i + 2 ) % m_hermiteCurvePoints] - points[i] ) * 0.5f;
		if ( ( i == 0 ) )
		{
			startVelocity = Vec2( 0.f, 0.f );
		}
		if ( ( i == m_hermiteCurvePoints - 1 ) )
		{
			endVelocity = Vec2( 0.f, 0.f );
			endPos = points[m_hermiteCurvePoints - 1];
		}
		m_hermiteCurves.push_back( CubicHermiteCurve2D( startPos, startVelocity, endPos, endVelocity ) );
	}
}

//------------------------------------------------------------------------------
void Game2DCurves::RenderBezierCurve( int numSamples /*= 64 */ ) const
{
	std::vector<Vertex> curveVerts;

	for ( int i = 0; i < numSamples; ++i )
	{
		float t0 = static_cast< float >( i ) / static_cast< float >( numSamples );
		float t1 = static_cast< float >( i + 1 ) / static_cast< float >( numSamples );

		Vec2 point0 = m_bezierCurve.EvaluateAtParametric( t0 );
		Vec2 point1 = m_bezierCurve.EvaluateAtParametric( t1 );

		AddVertsForLineSegment2D( curveVerts, point0, point1,  Vec2( .25f, .25f ), Rgba8::GREEN );
	}

	std::vector<Vertex> guideLineVerts;
	AddVertsForLineSegment2D( guideLineVerts, m_bezierCurve.GetStartPos(), m_bezierCurve.GetGuidePos1(), Vec2( .25f, .25f ), Rgba8( 128, 128, 128, 128 ) );
	AddVertsForLineSegment2D( guideLineVerts, m_bezierCurve.GetGuidePos2(), m_bezierCurve.GetEndPos(), Vec2( .25f, .25f ), Rgba8( 128, 128, 128, 128 ) );
	AddVertsForLineSegment2D( guideLineVerts, m_bezierCurve.GetGuidePos2(), m_bezierCurve.GetGuidePos1(), Vec2( .25f, .25f ), Rgba8( 128, 128, 128, 128 ) );

	std::vector<Vertex> controlPointVerts;
	AddVertsForDisc2D( controlPointVerts, m_bezierCurve.GetStartPos(), 0.5f, Rgba8::BLUE );
	AddVertsForDisc2D( controlPointVerts, m_bezierCurve.GetGuidePos1(), 0.5f, Rgba8::BLUE );
	AddVertsForDisc2D( controlPointVerts, m_bezierCurve.GetGuidePos2(), 0.5f, Rgba8::BLUE );
	AddVertsForDisc2D( controlPointVerts,  m_bezierCurve.GetEndPos(), 0.5f, Rgba8::BLUE );

	float parametricTime = fmodf( static_cast< float >( g_engine->m_systemClock->GetTotalSeconds() ) * 0.5f, 1.f );
	Vec2 parametricPoint = m_bezierCurve.EvaluateAtParametric( parametricTime );

	float curveLength = m_bezierCurve.GetApproximateLength( numSamples );
	float curveTime = fmodf( static_cast< float >( g_engine->m_systemClock->GetTotalSeconds() ), 2.f );
	float currentDistance = ( curveTime / 2.f ) * curveLength;
	Vec2 currentPoint = m_bezierCurve.EvaluateAtApproximateDistance( currentDistance, numSamples );

	std::vector<Vertex> pointVerts;
	AddVertsForDisc2D( pointVerts, currentPoint, 0.5f, Rgba8::WHITE );
	AddVertsForDisc2D( pointVerts, parametricPoint, 0.5f, Rgba8::YELLOW );

	g_engine->m_render->DrawVertexArray( static_cast< int >( guideLineVerts.size() ), guideLineVerts.data() );
	g_engine->m_render->DrawVertexArray( static_cast< int >( curveVerts.size() ), curveVerts.data() );
	g_engine->m_render->DrawVertexArray( static_cast< int >( controlPointVerts.size() ), controlPointVerts.data() );
	g_engine->m_render->DrawVertexArray( static_cast< int >( pointVerts.size() ), pointVerts.data() );
}

//-----------------------------------------------------------------------------------------------
void Game2DCurves::RenderHermiteCurve( int numSamples /*= 64 */ ) const
{

	for ( const CubicHermiteCurve2D& hermiteCurve : m_hermiteCurves )
	{
		std::vector<Vertex> curveVerts;

		for ( int i = 0; i < numSamples; ++i )
		{
			float t0 = static_cast< float >( i ) / static_cast< float >( numSamples );
			float t1 = static_cast< float >( i + 1 ) / static_cast< float >( numSamples );

			Vec2 point0 = hermiteCurve.EvaluateAtParametric(t0);
			Vec2 point1 = hermiteCurve.EvaluateAtParametric( t1 );

			AddVertsForLineSegment2D( curveVerts, point0, point1, Vec2( .25f, .25f ), Rgba8::GREEN );
		}
	
		std::vector<Vertex> arrowVerts;
		if ( hermiteCurve.GetStartVelocity().GetLengthSquared() > 0.f && hermiteCurve.GetEndVelocity().GetLengthSquared() > 0.f )
		{
			Vec2 startVelocity = hermiteCurve.GetStartPos() + hermiteCurve.GetStartVelocity();
			Vec2 endVelocity = hermiteCurve.GetEndPos() + hermiteCurve.GetEndVelocity();
			AddVertsForArrow2D( arrowVerts, hermiteCurve.GetStartPos(), startVelocity, 1.f, 1.f, Rgba8::RED );
			//AddVertsForArrow2D( arrowVerts, hermiteCurve.GetEndPos(), hermiteCurve.GetEndVelocity(), 1.f, 1.f, Rgba8::RED );
		}

		std::vector<Vertex> controlPointVerts;
		AddVertsForDisc2D( controlPointVerts, hermiteCurve.GetStartPos(), 0.5f, Rgba8::BLUE );
		AddVertsForDisc2D( controlPointVerts, hermiteCurve.GetEndPos(), 0.5f, Rgba8::BLUE );

		//float parametricTime = fmodf( static_cast< float >( g_engine->m_systemClock->GetTotalSeconds() ) * 0.5f, 1.f );
		//Vec2 parametricPoint = hermiteCurve.EvaluateAtParametric( parametricTime );
		//float curveLength = hermiteCurve.GetApproximateLength( numSamples );
		//float curveTime = fmodf( static_cast< float >( g_engine->m_systemClock->GetTotalSeconds() ), 2.f );
		//float currentDistance = ( curveTime / 2.f ) * curveLength;
		//Vec2 currentPoint = hermiteCurve.EvaluateAtApproximateDistance( currentDistance, numSamples );

		//std::vector<Vertex> pointVerts;
		//AddVertsForDisc2D( pointVerts, currentPoint, 0.5f, Rgba8::WHITE );
		//AddVertsForDisc2D( pointVerts, parametricPoint, 0.5f, Rgba8::YELLOW );

		g_engine->m_render->DrawVertexArray( static_cast< int >( arrowVerts.size() ), arrowVerts.data() );
		g_engine->m_render->DrawVertexArray( static_cast< int >( curveVerts.size() ), curveVerts.data() );
		g_engine->m_render->DrawVertexArray( static_cast< int >( controlPointVerts.size() ), controlPointVerts.data() );
		//g_engine->m_render->DrawVertexArray( static_cast< int >( pointVerts.size() ), pointVerts.data() );
	}
}
