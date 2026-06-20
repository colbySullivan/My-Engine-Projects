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
#include "Engine/Core/EventSystem.hpp"
#include "Game/App.hpp"
#include "ThirdParty/imgui/imgui.h"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Game/Player.hpp"
#include "Game/Prop.hpp"
#include "ChessPiece.hpp"
#include "Game/ChessPieceDefinition.hpp"
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
	CreateCameraModes();
	SubscribeEventCallbackFunction( "ChessBegin", Command_BeginGame );
	ChessPieceDefinition::InitializeChessPieceDefs();
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
	CreateDebugRenderObjects();
	RestartGame();
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
		for ( int i = 0; i < m_chessPieces.size(); ++i )
		{
			if ( m_chessPieces[i] != nullptr )
			{
				m_chessPieces[i]->Update();
			}
		}
		m_chessBoard->Update();
		UpdateImGui();

		unsigned char elapsedFraction = static_cast<unsigned char>(50 + ( m_cubeBlinkTimer->GetElapsedFraction() * 200 ));
		m_props[1]->m_color = Rgba8(elapsedFraction, elapsedFraction, elapsedFraction);
		if ( m_cubeBlinkTimer->DecrementPeriodIfElapsed() )
		{
			m_cubeBlinkTimer->Start();
		}
		if ( !m_currentCameraMode->freeCamera )
		{
			m_player->m_position = m_currentCameraMode->position;
			m_player->m_orientation = m_currentCameraMode->orientation;
			//g_engine->m_input->SetCursorMode( CursorMode::POINTER );
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
		/*for ( int propIndex = 0; propIndex < (int)m_props.size(); ++propIndex )
		{
			if ( m_props[propIndex] != nullptr )
			{
				m_props[propIndex]->Render();
			}
		}*/
		for ( int i = 0; i < m_chessPieces.size(); ++i )
		{
			if ( m_chessPieces[i] != nullptr )
			{
				m_chessPieces[i]->Render();
			}
		}
		m_chessBoard->Render();
		m_player->Render();
		g_engine->m_render->EndCamera( *m_player->m_worldCamera );
		RenderUI();
		DebugRenderWorld( *m_player->m_worldCamera );
		
	}

	g_engine->m_render->SetBlendMode( BlendMode::OPAQUE );
	g_engine->m_render->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
	g_engine->m_render->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
}

//-----------------------------------------------------------------------------------------------
void Game::Shutdown()
{
	for ( int i = 0; i < m_chessPieces.size(); ++i )
	{
		delete m_chessPieces[i];
		m_chessPieces[i] = nullptr;
	}
	m_chessPieces.clear();
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

	if (g_engine->m_input->WasKeyJustPressed( KEYCODE_F4 ))
	{
		m_currentCameraNum =  (m_currentCameraNum + 1 ) % m_cameraModes.size();
		m_currentCameraMode = m_cameraModes[m_currentCameraNum];
	}

	if (g_engine->m_input->WasKeyJustPressed(KEYCODE_F1))
	{
		m_showImGui = !m_showImGui;
	}

	if ( m_chessBoard && g_engine->m_input->WasKeyJustPressed( KEYCODE_LEFT_MOUSE ) )
	{
		//m_chessBoard->m_tryAllMovesRequested = true
		m_chessBoard->m_moveRequested = true;
	}

	DebugInput();

	g_engine->m_input->EndFrame();
}

//------------------------------------------------------------------------------
void Game::DebugInput()
{
	if ( g_engine->m_input->WasKeyJustPressed( '1' ) )
	{
		m_chessBoard->m_debugInt = 1;
		m_modeName = "Mode 1: Diffuse map texel only";
		//Mat44 toWorld = m_player->GetModelToWorldTransform();
		//Vec3 endPos = m_player->m_position + toWorld.GetIBasis3D() * 20.f;
		//DebugAddWorldCylinder( m_player->m_position, endPos, 0.0625f, 10.f, Rgba8( 255, 255, 0 ), Rgba8( 255, 255, 0 ), DebugRenderMode::X_RAY );
	}

	if ( g_engine->m_input->IsKeyDown( '2' ) )
	{
		m_chessBoard->m_debugInt = 2;
		m_modeName = "Mode 2: Normal map texel only";
		//Vec3 playerPos = m_player->m_position;
		//DebugAddWorldSphere( Vec3( playerPos.x, playerPos.y, 0.f ), 0.1f, 60.f, Rgba8( 150, 75, 0 ), Rgba8( 150, 75, 0 ), DebugRenderMode::USE_DEPTH );
	}

	if ( g_engine->m_input->WasKeyJustPressed( '3' ) )
	{
		m_chessBoard->m_debugInt = 3;
		m_modeName = "Mode 3: UV as Red, Green (with Blue=0)";
		//Vec3 forward = m_player->m_orientation.GetForwardDir_IFwd_JLeft_KUp();
		//Vec3 spawnPos = m_player->m_position + forward * 2.f;
		//DebugAddWorldWireSphere( spawnPos, 1.f, 5.f, Rgba8( 0, 255, 0 ), Rgba8( 255, 0, 0 ), DebugRenderMode::USE_DEPTH );
	}

	if ( g_engine->m_input->WasKeyJustPressed( '4' ) )
	{
		m_chessBoard->m_debugInt = 4;
		m_modeName = "Mode 4: World-space surface (vertex) Tangent vectors";
		//DebugAddBasis( m_player->GetModelToWorldTransform(), 5.f, 1.f, 0.1f, 1.f, 1.f, DebugRenderMode::USE_DEPTH );
	}

	if ( g_engine->m_input->WasKeyJustPressed( '5' ) )
	{
		m_chessBoard->m_debugInt = 5;
		m_modeName = "Mode 5: World-space surface (vertex) Bitangents";
		//Vec3 forward = m_player->m_orientation.GetForwardDir_IFwd_JLeft_KUp();
		//Vec3 spawnPos = m_player->m_position + forward * 2.f;
		//std::string posOrientText = Stringf( "Position: %5.2f, %5.2f, %5.2f Orientation: %5.2f, %5.2f, %5.2f", m_player->m_position.x, m_player->m_position.y, m_player->m_position.z, m_player->m_orientation.m_pitchDegrees, m_player->m_orientation.m_yawDegrees, m_player->m_orientation.m_rollDegrees );
		//DebugAddWorldBillboardText( posOrientText, spawnPos, 0.125f, Vec2( 0.5f, 0.5f ), 10.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 0, 0 ) );
	}

	if ( g_engine->m_input->WasKeyJustPressed( '6' ) )
	{
		m_chessBoard->m_debugInt = 6;
		m_modeName = "Mode 6: World-space surface (vertex) Normals";
		//float halfHeight = 0.5f;
		//Mat44 toWorld = m_player->GetModelToWorldTransform();
		//Vec3 upVector = toWorld.GetKBasis3D();
		//Vec3 startPos = m_player->m_position - ( upVector * halfHeight );
		//Vec3 endPos = m_player->m_position + ( upVector * halfHeight );
		//DebugAddWorldWireCylinder( startPos, endPos, 0.5f, 10.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 0, 0 ), DebugRenderMode::USE_DEPTH );
	}

	if ( g_engine->m_input->WasKeyJustPressed( '7' ) )
	{
		m_chessBoard->m_debugInt = 7;
		m_modeName = "Mode 7: World-space normal map normals";
		//std::string hudText = Stringf( "Camera orientation: %5.2f, %5.2f, %5.2f", m_player->m_orientation.m_pitchDegrees, m_player->m_orientation.m_yawDegrees, m_player->m_orientation.m_rollDegrees );
		//DebugAddMessage( hudText, 5.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 0, 0 ) );
	}

	if ( g_engine->m_input->WasKeyJustPressed( '8' ) )
	{
		m_chessBoard->m_debugInt = 8;
		m_modeName = "Mode 8: Surface-based lighting only";
	}

	if ( g_engine->m_input->WasKeyJustPressed( '9' ) )
	{
		m_chessBoard->m_debugInt = 9;
		m_modeName = "Mode 9: Normal-mapped lighting only";
	}

	if ( g_engine->m_input->WasKeyJustPressed( '0' ) )
	{
		m_chessBoard->m_debugInt = 0;
		m_modeName = "Mode 0: Normal rendering";
	}
}

//-----------------------------------------------------------------------------------------------
void Game::RenderUI() const
{
	float fps = 1.f / ( float )g_engine->m_systemClock->GetDeltaSeconds();
	float scale = (float)g_engine->m_systemClock->GetTimeScale();
	std::string hudText = Stringf( "Time: %.2f FPS: %6.1f Scale: %.2f", m_roundTime, fps, scale );
	DebugAddScreenText( hudText, AABB2( Vec2( 0.f, SCREEN_SIZE_Y - 25.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) ), 15.f, Vec2( 1.f, 0.5f ), 0.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 255, 255 ) );

	hudText = Stringf( "Player position: %5.2f, %5.2f, %5.2f", m_player->m_position.x, m_player->m_position.y, m_player->m_position.z );
	DebugAddScreenText( hudText, AABB2( Vec2( 0.f, SCREEN_SIZE_Y - 25.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) ), 10.f, Vec2( 0.f, 0.5f ), 0.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 255, 255 ) );

	hudText = Stringf( "Player orientation: %5.2f, %5.2f, %5.2f", m_player->m_orientation.m_yawDegrees, m_player->m_orientation.m_pitchDegrees, m_player->m_orientation.m_rollDegrees );
	DebugAddScreenText( hudText, AABB2( Vec2( 0.f, SCREEN_SIZE_Y - 45.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) ), 10.f, Vec2( 0.f, 0.5f ), 0.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 255, 255 ) );

	std::string cameraName = m_cameraModes[m_currentCameraNum]->modeName;
	hudText = Stringf( "Player camera mode (F4): %s", cameraName.c_str() );
	DebugAddScreenText( hudText, AABB2( Vec2( 0.f, SCREEN_SIZE_Y - 65.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) ), 10.f, Vec2( 0.f, 0.5f ), 0.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 255, 255 ) );

	DebugAddScreenText( m_modeName, AABB2( Vec2( 0.f, SCREEN_SIZE_Y - 85.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) ), 10.f, Vec2( 0.f, 0.5f ), 0.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 255, 255 ) );

	DebugRenderScreen( *m_screenCamera );
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
	char title[32] = "NetChess3D";
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
	//Prop* box = new Prop( this );
	//box->MakeCube( Rgba8( 255, 0, 0 ), Rgba8( 0, 255, 255 ), Rgba8( 0, 255, 0 ), Rgba8( 255, 0, 255 ), Rgba8( 255, 255, 0 ), Rgba8( 0, 0, 255 ) );
	//box->m_position = Vec3( 2.f, 2.f, 0.f );
	//box->m_angularVelocity.m_pitchDegrees = 30.f;
	//box->m_angularVelocity.m_rollDegrees = 30.f;
	//m_props.push_back( box );

	//Prop* newBox = new Prop( this );
	//newBox->MakeCube( Rgba8( 255, 0, 0 ), Rgba8( 0, 255, 255 ), Rgba8( 0, 255, 0 ), Rgba8( 255, 0, 255 ), Rgba8( 255, 255, 0 ), Rgba8( 0, 0, 255 ) );
	//newBox->m_position = Vec3( -2.f, -2.f, 0.f );
	//m_props.push_back( newBox );

	//Prop* sphere = new Prop( this );
	//sphere->MakeSphere( Vec3( 10, -5, 1 ), 1.f, 32, 16 );
	//sphere->m_texture = m_testTexture;
	//m_props.push_back( sphere );

	Prop* xAxisLines = new Prop( this );
	for (int xGridIndex = 0; xGridIndex < 100 ; ++xGridIndex)
	{
		Vec3 linePosition = Vec3( -50.f + ( xGridIndex ), 0.f, 0.f );
		if ( xGridIndex % 5 == 0 )
		{
			xAxisLines->MakeCubeAtPos( Rgba8( 0, 255, 0 ), Vec3( 0.1f, 100.f, 0.1f ), linePosition );
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
			yAxisLines->MakeCubeAtPos( Rgba8( 255, 0, 0 ), Vec3( 100.f, 0.1f, 0.1f ), linePosition );
		}
		else
		{
			yAxisLines->MakeCubeAtPos( Rgba8( 120, 120, 120 ), Vec3( 100.f, 0.05f, 0.05f ), linePosition );
		}
	}
	m_props.push_back( yAxisLines );

	//Prop* newCylinder = new Prop( this );
	//newCylinder->MakeCylinder( Vec3( -10.f, 5.f, 0.f ), Vec3( -10.f, 5.f, 2.f ), 0.5f, 32 );
	//newCylinder->m_texture = m_testTexture;
	//m_props.push_back( newCylinder );

	//Prop* newCone = new Prop( this );
	//newCone->MakeCone( Vec3( 10.f, 10.f, 0.f ), Vec3( 10.f, 10.f, 2.f ), 0.5f, 32 );
	//newCone->m_texture = m_testTexture;
	//m_props.push_back( newCone );
}

//-----------------------------------------------------------------------------------------------
void Game::CreateDebugRenderObjects()
{
	Mat44 worldBasis;
	DebugAddWorldBasis( worldBasis, -1.f, DebugRenderMode::USE_DEPTH );
}

//------------------------------------------------------------------------------
void Game::CreateChessPieces()
{
	ChessPieceDefinition const* kingDef = ChessPieceDefinition::GetByName( "King" );
	ChessPieceDefinition const* queenDef = ChessPieceDefinition::GetByName( "Queen" );
	ChessPieceDefinition const* rookDef = ChessPieceDefinition::GetByName( "Rook" );
	ChessPieceDefinition const* knightDef = ChessPieceDefinition::GetByName( "Knight" );
	ChessPieceDefinition const* bishopDef = ChessPieceDefinition::GetByName( "Bishop" );
	ChessPieceDefinition const* pawnDef = ChessPieceDefinition::GetByName( "Pawn" );

	if ( rookDef )
	{
		m_chessPieces.push_back( new ChessPiece( this, rookDef, Vec3( 0.5f, 0.5f, 0.f ), 1 ) );
		m_chessPieces.push_back( new ChessPiece( this, rookDef, Vec3( 7.5f, 0.5f, 0.f ), 1 ) );

		m_chessPieces.push_back( new ChessPiece( this, rookDef, Vec3( 0.5f, 7.5f, 0.f ), 2 ) );
		m_chessPieces.push_back( new ChessPiece( this, rookDef, Vec3( 7.5f, 7.5f, 0.f ), 2 ) );
	}
	if ( knightDef )
	{
		m_chessPieces.push_back( new ChessPiece( this, knightDef, Vec3( 1.5f, 0.5f, 0.f ), 1 ) );
		m_chessPieces.push_back( new ChessPiece( this, knightDef, Vec3( 6.5f, 0.5f, 0.f ), 1 ) );

		m_chessPieces.push_back( new ChessPiece( this, knightDef, Vec3( 1.5f, 7.5f, 0.f ), 2 ) );
		m_chessPieces.push_back( new ChessPiece( this, knightDef, Vec3( 6.5f, 7.5f, 0.f ), 2 ) );
	}
	if ( bishopDef ) 
	{
		m_chessPieces.push_back( new ChessPiece( this, bishopDef, Vec3( 2.5f, 0.5f, 0.f ), 1 ) );
		m_chessPieces.push_back( new ChessPiece( this, bishopDef, Vec3( 5.5f, 0.5f, 0.f ), 1 ) );

		m_chessPieces.push_back( new ChessPiece( this, bishopDef, Vec3( 2.5f, 7.5f, 0.f ), 2 ) );
		m_chessPieces.push_back( new ChessPiece( this, bishopDef, Vec3( 5.5f, 7.5f, 0.f ), 2 ) );
	}
	if ( queenDef )
	{
		m_chessPieces.push_back( new ChessPiece( this, queenDef, Vec3( 3.5f, 0.5f, 0.f ), 1 ) );

		m_chessPieces.push_back( new ChessPiece( this, queenDef, Vec3( 3.5f, 7.5f, 0.f ), 2 ) );
	}
	if ( kingDef )
	{
		m_chessPieces.push_back( new ChessPiece( this, kingDef, Vec3( 4.5f, 0.5f, 0.f ), 1 ) );

		m_chessPieces.push_back( new ChessPiece( this, kingDef, Vec3( 4.5f, 7.5f, 0.f ), 2 ) );
	}

	if ( pawnDef )
	{
		for ( int pawnColumn = 0; pawnColumn < 8; pawnColumn++ )
		{
			m_chessPieces.push_back( new ChessPiece( this, pawnDef, Vec3( ( float )pawnColumn + 0.5f, 1.5f, 0.f ), 1 ) );

			m_chessPieces.push_back( new ChessPiece( this, pawnDef, Vec3( ( float )pawnColumn + 0.5f, 6.5f, 0.f ), 2 ) );
		}
	}
	m_chessBoard->PrintBoardStateToConsole();
}

//------------------------------------------------------------------------------
void Game::CreateCameraModes()
{
	CameraMode* playerCameraMode1 = new CameraMode;
	playerCameraMode1->freeCamera = true;
	playerCameraMode1->position = Vec3( 4, -3, 4 );
	playerCameraMode1->orientation = EulerAngles( 90.f, 30.f, 0.f );
	playerCameraMode1->modeName = "Player One Camera";
	m_cameraModes.push_back( playerCameraMode1 );

	CameraMode* playerCameraMode2 = new CameraMode;
	playerCameraMode2->position = Vec3( 4, 11, 4 );
	playerCameraMode2->orientation = EulerAngles( -90.f, 30.f, 0.f );
	playerCameraMode2->freeCamera = true;
	playerCameraMode2->modeName = "Payer Two Camera";
	m_cameraModes.push_back( playerCameraMode2 );

	CameraMode* playerCameraFlyOver = new CameraMode;
	playerCameraFlyOver->position = Vec3( 4, 4, 9 );
	playerCameraFlyOver->orientation = EulerAngles( 90.f, 90.f, 0.f );
	playerCameraFlyOver->modeName = "Fly Over Camera";
	m_cameraModes.push_back( playerCameraFlyOver );

	CameraMode* playerCameraModeFree = new CameraMode;
	playerCameraModeFree->freeCamera = true;
	playerCameraModeFree->modeName = "Player Free Camera";
	m_cameraModes.push_back( playerCameraModeFree );

	m_currentCameraMode = m_cameraModes[m_currentCameraNum];
}

//-----------------------------------------------------------------------------------------------
bool Game::Command_BeginGame( [[maybe_unused]] EventArgs& args )
{
	if ( g_app->m_game->m_chessBoard->m_gameOver )
	{
		g_app->m_game->RestartGame();
	}
	else
	{
		g_engine->m_console->AddLine( DevConsole::ERROR_COLOR, "ChessBegin: Game already started" );
	}
	return false;
}

//-----------------------------------------------------------------------------------------------
void Game::CreateImGuiStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	// Base colors
	const ImVec4 bg_color = ImVec4( 0.4f, 0.5f, 0.4f, 1.00f );
	const ImVec4 bg_color_light = ImVec4( 0.2f, 0.4f, 0.2f, 1.00f );
	const ImVec4 accent = ImVec4( 0.2f, 1.f, 0.2f, 1.f );
	const ImVec4 accent_light = ImVec4( 0.f, 1.f, 0.f, 1.f );
	const ImVec4 accent_dark = ImVec4( 0.4f, 1.f, 0.5f, 1.f );

	// Window
	colors[ImGuiCol_WindowBg] = bg_color;
	colors[ImGuiCol_ChildBg] = bg_color;
	colors[ImGuiCol_PopupBg] = bg_color_light;

	// Text
	colors[ImGuiCol_Text] = ImVec4( 0.95f, 0.96f, 0.98f, 1.00f );
	colors[ImGuiCol_TextDisabled] = ImVec4( 0.50f, 0.50f, 0.50f, 1.00f );

	// Frames
	colors[ImGuiCol_FrameBg] = bg_color_light;
	colors[ImGuiCol_FrameBgHovered] = accent;
	colors[ImGuiCol_FrameBgActive] = accent_dark;

	// Buttons
	colors[ImGuiCol_Button] = accent;
	colors[ImGuiCol_ButtonHovered] = accent_light;
	colors[ImGuiCol_ButtonActive] = accent_dark;

	// Headers
	colors[ImGuiCol_Header] = accent;
	colors[ImGuiCol_HeaderHovered] = accent_light;
	colors[ImGuiCol_HeaderActive] = accent_dark;

	// Tabs
	colors[ImGuiCol_Tab] = bg_color_light;
	colors[ImGuiCol_TabHovered] = accent_light;
	colors[ImGuiCol_TabSelected] = accent;
	colors[ImGuiCol_TabSelectedOverline] = accent_light;

	// Title
	colors[ImGuiCol_TitleBg] = bg_color;
	colors[ImGuiCol_TitleBgActive] = bg_color_light;
	colors[ImGuiCol_TitleBgCollapsed] = bg_color;

	// Scrollbar
	colors[ImGuiCol_ScrollbarBg] = bg_color;
	colors[ImGuiCol_ScrollbarGrab] = bg_color_light;
	colors[ImGuiCol_ScrollbarGrabHovered] = accent;
	colors[ImGuiCol_ScrollbarGrabActive] = accent_dark;

	// Style properties
	style.WindowRounding = 6.0f;
	style.FrameRounding = 4.0f;
	style.ScrollbarRounding = 6.0f;
	style.GrabRounding = 4.0f;
	style.TabRounding = 4.0f;
}

//------------------------------------------------------------------------------
void Game::RemoveChessPiece( ChessPiece* piece )
{
	for ( int i = 0; i < m_chessPieces.size(); ++i )
	{
		if ( m_chessPieces[i] == piece )
		{
			m_chessPieces.erase( m_chessPieces.begin() + i );
			return;
		}
	}
}

//------------------------------------------------------------------------------
void Game::RestartGame()
{
	for ( int i = 0; i < m_chessPieces.size(); ++i )
	{
		delete m_chessPieces[i];
		m_chessPieces[i] = nullptr;
	}
	m_chessPieces.clear();

	delete m_chessBoard;
	m_chessBoard = new ChessBoard( this );
	CreateChessPieces();
	ChangePlayerCamera( 1 );
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateImGui()
{
	if ( !m_showImGui )
		return;

	CreateImGuiStyle();

	ImGui::Begin( "Shader Debug (F1 to hide)", &m_showImGui );

	ImGui::SeparatorText( "Debug View Mode" );
	const char* modeNames[] = {
		"0: Normal Rendering",
		"1: Diffuse Map Only",
		"2: Normal Map Only",
		"3: UV Coordinates",
		"4: World Tangents",
		"5: World Bitangents",
		"6: World Normals",
		"7: Normal Map (World)",
		"8: Surface Lighting Only",
		"9: Normal Map Lighting Only"
	};
	ImGui::Combo( "Mode", &m_chessBoard->m_debugInt, modeNames, 10 );

	ImGui::SeparatorText( "Debug Values" );
	ImGui::SliderFloat( "debugFloat", &m_chessBoard->m_debugFloat, 0.0f, 1.0f );

	ImGui::SeparatorText( "Sun Dir" );
	ImGui::SliderFloat( "Sun Direction x", &m_chessBoard->m_sunDir.x, -90.0f, 90.0f );
	ImGui::SliderFloat( "Sun Direction y", &m_chessBoard->m_sunDir.y, -90.0f, 90.0f );
	ImGui::SliderFloat( "Sun Direction z", &m_chessBoard->m_sunDir.z, -90.0f, 90.0f );

	ImGui::SeparatorText( "Sun Color" );
	float color[4];
	m_chessBoard->m_sunColor.GetAsFloats( color );
	if ( ImGui::ColorEdit4( "Sun Color", color ) )
	{
		m_chessBoard->m_sunColor = Rgba8(
			( unsigned char )( color[0] * 255.f ),
			( unsigned char )( color[1] * 255.f ),
			( unsigned char )( color[2] * 255.f ),
			( unsigned char )( color[3] * 255.f ) );
	}

	ImGui::End();
}

//-----------------------------------------------------------------------------------------------
void Game::ChangePlayerCamera( int player )
{
	m_currentCameraMode = m_cameraModes[ player - 1 ];
	m_player->m_position = m_currentCameraMode->position;
	m_player->m_orientation = m_currentCameraMode->orientation;
}
