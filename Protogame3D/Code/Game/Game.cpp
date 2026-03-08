#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Game/Player.hpp"
#include "Game/Prop.hpp"
#include <ThirdParty/stb/stb_image.h>

RandomNumberGenerator g_rng;

//-----------------------------------------------------------------------------------------------
Game::Game()
{
	m_screenCamera = new Camera;


	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	m_roundNumber = 1;
	g_testFont = g_engine->m_render->CreateOrGetBitmapFont( "Data/Fonts/SquirrelFixedFont" );
	m_testTexture = g_engine->m_render->CreateTextureFromImage( "Data/Textures/TestUV.png" );
	m_gameClock = new Clock( *g_engine->m_systemClock );
	Vec2 worldCenter( WORLD_SIZE_X * 0.5f, WORLD_SIZE_Y * 0.5f );
	m_player = new Player( this );
	m_player->m_position = Vec3( 0.f, 0.f, 1.f);
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );
	CreateProps();
}

//-----------------------------------------------------------------------------------------------
Game::~Game()
{
	delete g_engine;
	delete m_screenCamera;
	g_engine = nullptr;
	m_screenCamera = nullptr;
}

//-----------------------------------------------------------------------------------------------
// Game Loop
//-----------------------------------------------------------------------------------------------
void Game::Startup()
{
	m_isPaused = false;

	m_cubeBlinkTimer = new Timer( 3.5f );
	m_cubeBlinkTimer->Start();

	//DebugAddWorldWireCylinder(Vec3( 2.f, 2.f, -2.f ), Vec3( 2.f, 2.f, 2.f ), 0.1f, 5.f, Rgba8(0,255,100), Rgba8(255,255,255), DebugRenderMode::X_RAY);
	//DebugAddWorldCylinder(Vec3( 0.f, 3.f, -2.f ), Vec3( 2.f, 2.f, 2.f ), 0.1f, 5.f, Rgba8(0,255,100), Rgba8(255,255,255), DebugRenderMode::X_RAY);
	////DebugAddWorldSphere( Vec3( 0.f, 0.f, 0.f ), 1.f, 5.f, Rgba8( 255, 100, 0 ), Rgba8( 255, 100, 0 ), DebugRenderMode::X_RAY );
	//DebugAddWorldWireSphere( Vec3( 0.f, 0.f, 0.f ), 1.f, 5.f, Rgba8( 255, 100, 0 ), Rgba8( 255, 100, 0 ), DebugRenderMode::X_RAY );
	DebugAddScreenText( "Hello World", AABB2( Vec2( 0.f, SCREEN_SIZE_Y - 35.f ), Vec2( SCREEN_SIZE_X * 0.5, SCREEN_SIZE_Y ) ), 30.f, Vec2( 0.f, 0.5f ), 5.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 255, 255 ) );
}

//-----------------------------------------------------------------------------------------------
void Game::Update()
{
	XboxController const& controller = g_engine->m_input->GetController( 0 );

	float deltaSeconds = ( float )m_gameClock->GetDeltaSeconds();

	UpdateKeyboardInput( controller );

	if ( m_currentGameState != m_nextGameState )
	{
		m_currentGameState = m_nextGameState;
	}

	if ( m_currentGameState == GAMESTATE_ATTRACT )
	{
		UpdateAttractMode( deltaSeconds );
	}

	if ( m_currentGameState == GAMESTATE_PLAY )
	{
		if ( m_isSlowMo ) // T pressed
		{
			deltaSeconds = 1.f / 600.f; // Run at 1/10th the speed
		}

		if ( !m_isPaused || m_pauseAfterNextUpdate )
		{
			m_pauseAfterNextUpdate = false; // Reset run token for simulation step
		}
		m_roundTime += deltaSeconds;
		for ( int propIndex = 0; propIndex < (int)m_props.size(); ++propIndex )
		{
			if ( m_props[propIndex] != nullptr )
			{
				m_props[propIndex]->Update( deltaSeconds );
			}
		}

		unsigned char elapsedFraction = static_cast<unsigned char>(50 + ( m_cubeBlinkTimer->GetElapsedFraction() * 200 ));
		m_props[1]->m_color = Rgba8(elapsedFraction, elapsedFraction, elapsedFraction);
		if ( m_cubeBlinkTimer->DecrementPeriodIfElapsed() )
		{
			m_cubeBlinkTimer->Start();
		}
		m_player->Update( (float) g_engine->m_systemClock->GetDeltaSeconds() );
	}
}

//-----------------------------------------------------------------------------------------------
void Game::Render() const
{
	//g_engine->m_render->BindTexture( nullptr );
	g_engine->m_render->BeginCamera( *m_player->m_worldCamera );
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;

	if ( m_currentGameState == GAMESTATE_ATTRACT )
	{
		Rgba8 backgroundColor = Rgba8( static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 255.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 255.f ) ); // Suppresses error with conversion
		g_engine->m_render->ClearScreen( backgroundColor );
		RenderAttractMode();
	}

	if ( m_currentGameState == GAMESTATE_ATTRACT )
	{
		return;
	}

	if ( m_currentGameState == GAMESTATE_PLAY )
	{
		Rgba8 backgroundColor = Rgba8( static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ) ); // Suppresses error with conversion
		g_engine->m_render->ClearScreen( backgroundColor );
		for ( int propIndex = 0; propIndex < (int)m_props.size(); ++propIndex )
		{
			if ( m_props[propIndex] != nullptr )
			{
				m_props[propIndex]->Render();
			}
		}
		g_engine->m_render->EndCamera( *m_player->m_worldCamera );
		DebugRenderScreen( *m_screenCamera );
		DebugRenderWorld( *m_player->m_worldCamera );
		
	}

	g_engine->m_render->SetBlendMode( BlendMode::OPAQUE );
	g_engine->m_render->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
	g_engine->m_render->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
}

//-----------------------------------------------------------------------------------------------
void Game::Shutdown()
{

}

//-----------------------------------------------------------------------------------------------
// Input Handling
//-----------------------------------------------------------------------------------------------
void Game::UpdateKeyboardInput( XboxController const& controller )
{
	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_ESC ) || controller.WasButtonJustPressed( XboxButtonID::BACK ) )
	{
		if ( m_currentGameState == GAMESTATE_ATTRACT )
		{
			m_isQuitting = true;
			return;
		}
	}

	if ( ( g_engine->m_input->WasKeyJustPressed( KEYCODE_ESC ) || controller.WasButtonJustPressed( XboxButtonID::BACK ) ) && m_currentGameState != GAMESTATE_ATTRACT )
	{
		m_nextGameState = GAMESTATE_ATTRACT;
	}

	m_isSlowMo = g_engine->m_input->IsKeyDown('T');  // Slows simulation time to 1/10th the normal rate

	if (g_engine->m_input->WasKeyJustPressed('P') || controller.WasButtonJustPressed(XboxButtonID::START)) // Pauses game
	{
		m_isPaused = !m_isPaused; // Switch pause
	}
	if (g_engine->m_input->WasKeyJustPressed('O')) // Runs a single unpaused Update (simulation step) and then pauses.
	{
		m_isPaused = true;
		m_pauseAfterNextUpdate = true; // Consumed to false after one run of update
	}
	if ( g_engine->m_input->WasKeyJustPressed( ' ' ) || g_engine->m_input->WasKeyJustPressed( 'N' ) || controller.WasButtonJustPressed( XboxButtonID::START ) )
	{
		if ( m_currentGameState != GAMESTATE_PLAY )
		{
			m_nextGameState = GAMESTATE_PLAY;
			Startup();
		}
	}

	if (g_engine->m_input->WasKeyJustPressed('I'))
	{

	}

	if (g_engine->m_input->WasKeyJustPressed(KEYCODE_F1))
	{
		g_drawDebug = !g_drawDebug;
	}

	DebugInput();

	g_engine->m_input->EndFrame();
}

//------------------------------------------------------------------------------
void Game::DebugInput()
{
	if ( g_engine->m_input->WasKeyJustPressed( '1' ) )
	{
		Mat44 toWorld = m_player->GetModelToWorldTransform();
		DebugAddWorldCylinder(m_player->m_position, toWorld.GetIBasis3D() * 20.f, 0.0625f, 10.f, Rgba8(255, 255, 0), Rgba8(255, 255, 0), DebugRenderMode::X_RAY);
	}

	if ( g_engine->m_input->IsKeyDown( '2' ) )
	{
		Mat44 toWorld = m_player->GetModelToWorldTransform();
		Vec3 playerPos = m_player->m_position;
		DebugAddWorldSphere( Vec3( playerPos.x, playerPos.y, 0.f ), 0.1f, 60.f, Rgba8( 150, 75, 0 ), Rgba8( 150, 75, 0 ), DebugRenderMode::USE_DEPTH );
	}
}

//-----------------------------------------------------------------------------------------------
void Game::RenderUI() const
{
	//Camera attractCamera;

	//char textBuffer[64];
	//snprintf(textBuffer, sizeof(textBuffer), "Time: %.2f", m_roundTime);
	//RenderText(textBuffer, Vec2(700.f, 750.f), 20.f, Rgba8(50, 150, 255, 255));

}

//-----------------------------------------------------------------------------------------------
void Game::RenderText(const char text[] , Vec2 pos, float height, Rgba8 color) const
{
	std::vector<Vertex> textVerts;
	AddVertsForTextTriangles2D( textVerts, text, pos, height, color, 1.f );
	g_engine->m_render->DrawVertexArray( ( int )textVerts.size(), textVerts.data() );
}

//-----------------------------------------------------------------------------------------------
// Attract Mode
//-----------------------------------------------------------------------------------------------
void Game::UpdateAttractMode(float deltaSeconds)
{
	m_roundTime = 0.f;

	m_shipAnimationTimer += deltaSeconds;

	if ( m_shipAnimationTimer > SHIP_ANIMATION_DURATION )
	{
		m_shipAnimationTimer = 0.0f;
	}
	UpdateBlackHole();
}

//-----------------------------------------------------------------------------------------------
void Game::RenderAttractMode() const
{
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );
	g_engine->m_render->BeginCamera( *m_screenCamera );
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_NONE;

	// Background
	Vertex background[6];
	background[0].m_position = Vec3( 0.f, 0.f, 0.f );
	background[1].m_position = Vec3( 0.f, SCREEN_SIZE_Y, 0.f );
	background[2].m_position = Vec3( SCREEN_SIZE_X, SCREEN_SIZE_Y, 0.f );

	background[3].m_position = Vec3( 0.f, 0.f, 0.f );
	background[4].m_position = Vec3( SCREEN_SIZE_X, 0.f, 0.f );
	background[5].m_position = Vec3( SCREEN_SIZE_X, SCREEN_SIZE_Y, 0.f );
	for ( int vertIndex = 0; vertIndex < 6; ++vertIndex )
	{
		background[vertIndex].m_color = Rgba8( 255, 255, 255, 255 );
	}
	TransformVertexArrayXY3D(
		6,
		background,
		1.0f,
		0.f,
		Vec2( 0.f, 0.f ) );
	g_engine->m_render->DrawVertexArray( 6, background );

	// Black hole
	Vertex tempHoleWorldVerts[NUM_BLACK_HOLE_VERTS];
	for ( int vertIndex = 0; vertIndex < NUM_BLACK_HOLE_VERTS; ++vertIndex )
	{
		tempHoleWorldVerts[vertIndex] = m_blackHoleVerts[vertIndex];
	}
	TransformVertexArrayXY3D(
		NUM_BLACK_HOLE_VERTS,
		tempHoleWorldVerts,
		150.0f,
		0.f,
		Vec2( 800.f, 400.f ) );
	g_engine->m_render->DrawVertexArray( NUM_BLACK_HOLE_VERTS, tempHoleWorldVerts );

	// Title
	char title[32] = "Protogame3D";
	for ( int charIndex = 0; charIndex < 18; ++charIndex )
	{
		char singleChar[2] = { title[charIndex], '\0' };
		int offsetColorR =  g_rng.RollRandomIntInRange(200, 255);
		int offsetColorG =  g_rng.RollRandomIntInRange(200, 255);
		int offsetColorB =  g_rng.RollRandomIntInRange(0, 255);
		RenderText( singleChar, Vec2( 475.f + charIndex * 40.f, 100.f ), 40.0f, Rgba8( static_cast<unsigned char>(offsetColorR), static_cast<unsigned char>(offsetColorG), static_cast<unsigned char>(offsetColorB) ) );

	}
	char nameTitle[96] = "Made by Colby Sullivan";
	for ( int charIndex = 0; charIndex < 22; ++charIndex )
	{
		char singleChar[2] = { nameTitle[charIndex], '\0' };
		int offsetColorR = g_rng.RollRandomIntInRange( 200, 255 );
		int offsetColorG = g_rng.RollRandomIntInRange( 200, 255 );
		int offsetColorB = g_rng.RollRandomIntInRange( 0, 255 );
		RenderText( singleChar, Vec2( 390.f + charIndex * 40.f, 50.f ), 40.0f, Rgba8( static_cast< unsigned char >( offsetColorR ), static_cast< unsigned char >( offsetColorG ), static_cast< unsigned char >( offsetColorB ) ) );

	}

	std::vector<Vertex> testTextureVerts;
	AABB2 texturedAABB2( 0.f, 0.f, 512.f, 512.f );
	AddVertsForAABB2D( testTextureVerts, texturedAABB2, Rgba8( 255, 255, 255, 255 ) ); // This should now set UVs on each Vertex!!

	g_engine->m_render->EndCamera( *m_screenCamera );
}

//-----------------------------------------------------------------------------------------------
// Utility Functions
//-----------------------------------------------------------------------------------------------

void Game::UpdateBlackHole()
{
	float holeRadii[NUM_BLACK_HOLE_SIDES] = {};
	for ( int sideNum = 0; sideNum < NUM_BLACK_HOLE_SIDES; ++sideNum )
	{
		holeRadii[sideNum] = g_rng.RollRandomFloatInRange( 1.9f, 2.0f );
	}

	// Precompute 2D vertex offsets
	constexpr float degreesPerBlackHoleSide = 360.f / static_cast<float>(NUM_BLACK_HOLE_SIDES);
	Vec2 blackholeLocalVertPositions[NUM_BLACK_HOLE_SIDES] = {};

	for ( int sideNum = 0; sideNum < NUM_BLACK_HOLE_SIDES; ++sideNum )
	{
		float degrees = degreesPerBlackHoleSide * static_cast<float>(sideNum);
		float radius = holeRadii[sideNum];
		blackholeLocalVertPositions[sideNum].x = radius * CosDegrees( degrees );
		blackholeLocalVertPositions[sideNum].y = radius * SinDegrees( degrees );
	}

	// Build triangles
	for ( int triNum = 0; triNum < NUM_BLACK_HOLE_TRIS; ++triNum )
	{
		int startRadiusIndex = triNum;
		int endRadiusIndex = ( triNum + 1 ) % NUM_BLACK_HOLE_SIDES;
		int firstVertIndex = ( triNum * 3 ) + 0;
		int secondVertIndex = ( triNum * 3 ) + 1;
		int thirdVertIndex = ( triNum * 3 ) + 2;
		Vec2 secondVertOfs = blackholeLocalVertPositions[startRadiusIndex];
		Vec2 thirdVertOfs = blackholeLocalVertPositions[endRadiusIndex];
		m_blackHoleVerts[firstVertIndex].m_position = Vec3( 0.f, 0.f, 0.f );
		m_blackHoleVerts[secondVertIndex].m_position = Vec3( secondVertOfs.x, secondVertOfs.y, 0.f );
		m_blackHoleVerts[thirdVertIndex].m_position = Vec3( thirdVertOfs.x, thirdVertOfs.y, 0.f );
	}

	// Set colors
	for ( int triNum = 0; triNum < NUM_BLACK_HOLE_TRIS; ++triNum )
	{
		int firstVertIndex = ( triNum * 3 ) + 0;
		int secondVertIndex = ( triNum * 3 ) + 1;
		int thirdVertIndex = ( triNum * 3 ) + 2;

		// Center vertex
		m_blackHoleVerts[firstVertIndex].m_color = Rgba8( 0, 0, 0, 255 );

		// Edge vertices
		m_blackHoleVerts[secondVertIndex].m_color = Rgba8( 255, 255, 255, 255 );
		m_blackHoleVerts[thirdVertIndex].m_color = Rgba8( 255, 255, 255, 255 );
	}
}

//-----------------------------------------------------------------------------------------------
void Game::CreateProps()
{
	Prop* box = new Prop( this );
	box->MakeCube( Rgba8( 255, 0, 0 ), Rgba8( 0, 255, 255 ), Rgba8( 0, 255, 0 ), Rgba8( 255, 0, 255 ), Rgba8( 255, 255, 0 ), Rgba8( 0, 0, 255 ) );
	box->m_position = Vec3( 2.f, 2.f, 0.f );
	box->m_angularVelocity.m_pitchDegrees = 30.f;
	box->m_angularVelocity.m_rollDegrees = 30.f;
	m_props.push_back( box );

	Prop* newBox = new Prop( this );
	newBox->MakeCube( Rgba8( 255, 0, 0 ), Rgba8( 0, 255, 255 ), Rgba8( 0, 255, 0 ), Rgba8( 255, 0, 255 ), Rgba8( 255, 255, 0 ), Rgba8( 0, 0, 255 ) );
	newBox->m_position = Vec3( -2.f, -2.f, 0.f );
	m_props.push_back( newBox );

	Prop* sphere = new Prop( this );
	sphere->MakeSphere( Vec3( 10, -5, 1 ), 1.f, 32, 16 );
	sphere->m_texture = m_testTexture;
	m_props.push_back( sphere );

	Prop* xAxisLines = new Prop( this );
	for (int xGridIndex = 0; xGridIndex < 100 ; ++xGridIndex)
	{
		Vec3 linePosition = Vec3( -50.f + ( xGridIndex ), 0.f, 0.f );
		if ( xGridIndex % 5 == 0 )
		{
			xAxisLines->MakeCubeAtPos( Rgba8( 255, 0, 0 ), Vec3( 0.1f, 100.f, 0.1f ), linePosition );
		}
		else
		{
			xAxisLines->MakeCubeAtPos( Rgba8( 120, 120, 120 ), Vec3( 0.05f, 100.f, 0.05f ), linePosition );
		}
	}
	m_props.push_back( xAxisLines );

	Prop* yAxisLines = new Prop( this );
	for ( int yGridIndex = 0; yGridIndex < 100; ++yGridIndex )
	{
		Vec3 linePosition = Vec3( 0.f, -50.f + ( yGridIndex ), 0.f );
		if ( yGridIndex % 5 == 0 )
		{
			yAxisLines->MakeCubeAtPos( Rgba8( 0, 255, 0 ), Vec3( 100.f, 0.1f, 0.1f ), linePosition );
		}
		else
		{
			yAxisLines->MakeCubeAtPos( Rgba8( 120, 120, 120 ), Vec3( 100.f, 0.05f, 0.05f ), linePosition );
		}
	}
	m_props.push_back( yAxisLines );

	Prop* newCylinder = new Prop( this );
	newCylinder->MakeCylinder( Vec3( -10.f, 5.f, 0.f ), Vec3( -10.f, 5.f, 2.f ), 0.5f, 32 );
	m_props.push_back( newCylinder );

	Prop* newCone = new Prop( this );
	newCone->MakeCone( Vec3( 10.f, 10.f, 0.f ), Vec3( 10.f, 10.f, 2.f ), 0.5f, 32 );
	newCone->m_texture = m_testTexture;
	m_props.push_back( newCone );
}
