#include "Game/Game2DCurves.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"

//-----------------------------------------------------------------------------------------------
Game2DCurves::Game2DCurves( App* app )
	: Game( app )
{

}

Game2DCurves::~Game2DCurves()
{
	Shutdown();
}

void Game2DCurves::Startup()
{
	Game::Startup();
}

void Game2DCurves::Shutdown()
{

}

void Game2DCurves::Update( float deltaSeconds )
{
	UpdateCameras( deltaSeconds );
	Game::UpdateKeyboardInput();
}

void Game2DCurves::Render() const
{
	Rgba8 backgroundColor = Rgba8( static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 255.f ) ); // Suppresses error with conversion

	g_engine->m_render->ClearScreen( backgroundColor );

	g_engine->m_render->BeginCamera( *m_worldCamera );

	RenderEasingGraph( 64 );

	Game::RenderGameText( GAMEMODE_2D_CURVES );

	g_engine->m_render->EndCamera( *m_worldCamera );
}

//------------------------------------------------------------------------------
void Game2DCurves::RenderEasingGraph( int numSamples ) const
{
	float graphTime = fmodf( static_cast< float >( g_engine->m_systemClock->GetTotalSeconds() ), 1.f );

	float padding = 10.f;
	float sideLength = 30.f;
	Vec2 graphMin = Vec2( padding, 100.f - padding - sideLength );
	Vec2 graphMax = Vec2( padding + sideLength, 100.f - padding );
	float graphWidth = graphMax.x - graphMin.x;
	float graphHeight = graphMax.y - graphMin.y;

	std::vector<Vertex> axisVerts;
	AddVertsForAABB2D( axisVerts, AABB2( graphMin, graphMax ), Rgba8( 120, 120, 120, 120 ) );
	g_engine->m_render->DrawVertexArray( static_cast< int >( axisVerts.size() ), axisVerts.data() );

	std::vector<Vertex> curveVerts;

	for ( int i = 0; i < numSamples - 1; ++i )
	{
		float t0 = static_cast< float >( i ) / static_cast< float >( numSamples - 1 );
		float t1 = static_cast< float >( i + 1 ) / static_cast< float >( numSamples - 1 );

		float easedValue0 = SmoothStep5( t0 );
		float easedValue1 = SmoothStep5( t1 );

		Vec2 point0 = Vec2( graphMin.x + t0 * graphWidth, graphMin.y + easedValue0 * graphHeight );
		Vec2 point1 = Vec2( graphMin.x + t1 * graphWidth, graphMin.y + easedValue1 * graphHeight );

		AddVertsForLineSegment2D( curveVerts, point0, point1, Vec2( .25f, .25f ), Rgba8::GREEN );
	}

	g_engine->m_render->DrawVertexArray( static_cast< int >( curveVerts.size() ), curveVerts.data() );

	float easedValue = SmoothStep5( graphTime );
	Vec2 currentPoint = Vec2( graphMin.x + graphTime * graphWidth, graphMin.y + easedValue * graphHeight );

	std::vector<Vertex> pointVerts;
	AddVertsForDisc2D( pointVerts, currentPoint, 0.5f, Rgba8::WHITE );
	g_engine->m_render->DrawVertexArray( static_cast< int >( pointVerts.size() ), pointVerts.data() );
}

