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
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"
#include <ThirdParty/stb/stb_image.h>
#include "TileDefinitions.hpp"

RandomNumberGenerator g_rng;
Game* g_game = nullptr;
XmlUtils m_xml;


//-----------------------------------------------------------------------------------------------
Game::Game()
{
	m_screenCamera = new Camera;
	g_game = this;

	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	m_roundNumber = 1;
	g_testFont = g_engine->m_render->CreateOrGetBitmapFont( "Data/Fonts/SquirrelFixedFont" );
	m_testTexture = g_engine->m_render->CreateTextureFromImage( "Data/Textures/TestUV.png" );
	m_gameClock = new Clock( *g_engine->m_systemClock );
	Vec2 worldCenter( WORLD_SIZE_X * 0.5f, WORLD_SIZE_Y * 0.5f );
	m_player = new Player( this );
	m_player->m_position = Vec3( 2.5f, 8.5, 0.5f );	
	m_player->m_orientation = EulerAngles( 0.f, 0.f, 0.f );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( g_gameConfig->GetValue( "screenSizeX", 0.f ), g_gameConfig->GetValue( "screenSizeY", 0.f ) ) );
	CreateProps();
}

//-----------------------------------------------------------------------------------------------
Game::~Game()
{
	delete m_teemoModel;
	m_teemoModel = nullptr;
	for ( int i = 0; i < m_maps.size(); ++i ) 
	{
		delete m_maps[i];
		m_maps[i] = nullptr;
	}
	m_maps.clear();
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
	// #todo there is an issue if you load and unload the game multiple times before exiting
	m_isPaused = false;
	TileDefinition::InitializeTileDefs();
	ConstructMapFromXML();
	if ( m_maps.size() > 0 )
	{
		m_currentMapNumber = 0;
		m_currentMap = m_maps[m_currentMapNumber];
	}
	//m_teemoModel = LoadOBJFromFile( "Data/Textures/summoner_rift.obj", g_engine->m_render ); // #TODO The world is not ready for summoners rift obj
	//m_teemoModel = LoadOBJFromFile( "Data/Textures/Veigar.obj", g_engine->m_render );
	m_teemoModel = LoadOBJFromFile( "Data/Textures/Teemo.obj", g_engine->m_render );
	m_teemoTexture = g_engine->m_render->CreateTextureFromImage( "Data/Textures/teemo_texture.png" );
	//m_teemoTexture = nullptr;
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
		m_player->Update( deltaSeconds );
		m_currentMap->Update();
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
		m_currentMap->Render();
		g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_NONE;
		Vec3 modelPosition = Vec3( 10.0f, 5.0f, 0.0f ); 
		float modelScale = 0.01f;                       
		float modelRotation = 90.0f;                    

		Mat44 modelMatrix;
		modelMatrix.SetTranslation3D( modelPosition );
		modelMatrix.AppendScaleUniform3D( modelScale );
		modelMatrix.AppendXRotation( modelRotation );
		g_engine->m_render->SetModelConstants( modelMatrix, Rgba8( 255, 255, 255, 255 ) );
		g_engine->m_render->BindTexture( m_teemoTexture );
		g_engine->m_render->DrawIndexBuffer( m_teemoModel->m_vbo, m_teemoModel->m_ibo, m_teemoModel->m_indexCount );

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
		Vec3 endPos = m_player->m_position + toWorld.GetIBasis3D() * 20.f;
		DebugAddWorldCylinder( m_player->m_position, endPos, 0.0625f, 10.f, Rgba8( 255, 255, 0 ), Rgba8( 255, 255, 0 ), DebugRenderMode::X_RAY );
	}

	if ( g_engine->m_input->IsKeyDown( '2' ) )
	{
		Vec3 playerPos = m_player->m_position;
		DebugAddWorldSphere( Vec3( playerPos.x, playerPos.y, 0.f ), 0.1f, 60.f, Rgba8( 150, 75, 0 ), Rgba8( 150, 75, 0 ), DebugRenderMode::USE_DEPTH );
	}

	if ( g_engine->m_input->WasKeyJustPressed( '3' ) )
	{
		Vec3 forward = m_player->m_orientation.GetForwardDir_IFwd_JLeft_KUp();
		Vec3 spawnPos = m_player->m_position + forward * 2.f;
		DebugAddWorldWireSphere( spawnPos, 1.f, 5.f, Rgba8( 0, 255, 0 ), Rgba8( 255, 0, 0 ), DebugRenderMode::USE_DEPTH );
	}

	if ( g_engine->m_input->WasKeyJustPressed( '4' ) )
	{
		DebugAddBasis( m_player->GetModelToWorldTransform(), 5.f, 1.f, 0.1f, 1.f, 1.f, DebugRenderMode::USE_DEPTH );
	}

	if ( g_engine->m_input->WasKeyJustPressed( '5' ) )
	{
		Vec3 forward = m_player->m_orientation.GetForwardDir_IFwd_JLeft_KUp();
		Vec3 spawnPos = m_player->m_position + forward * 2.f;
		std::string posOrientText = Stringf( "Position: %5.2f, %5.2f, %5.2f Orientation: %5.2f, %5.2f, %5.2f", m_player->m_position.x, m_player->m_position.y, m_player->m_position.z, m_player->m_orientation.m_pitchDegrees, m_player->m_orientation.m_yawDegrees, m_player->m_orientation.m_rollDegrees );
		DebugAddWorldBillboardText( posOrientText, spawnPos, 0.125f, Vec2( 0.5f, 0.5f ), 10.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 0, 0 ) );
	}

	if ( g_engine->m_input->WasKeyJustPressed( '6' ) )
	{
		float halfHeight = 0.5f;
		Mat44 toWorld = m_player->GetModelToWorldTransform();
		Vec3 upVector = toWorld.GetKBasis3D();
		Vec3 startPos = m_player->m_position - ( upVector * halfHeight );
		Vec3 endPos = m_player->m_position + ( upVector * halfHeight );
		DebugAddWorldWireCylinder( startPos, endPos, 0.5f, 10.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 0, 0 ), DebugRenderMode::USE_DEPTH );
	}

	if ( g_engine->m_input->WasKeyJustPressed( '7' ) )
	{
		std::string hudText = Stringf( "Camera orientation: %5.2f, %5.2f, %5.2f", m_player->m_orientation.m_pitchDegrees, m_player->m_orientation.m_yawDegrees, m_player->m_orientation.m_rollDegrees );
		DebugAddMessage( hudText, 5.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 0, 0 ) );
	}

	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F1 ) )
	{
		m_controlPlayerMode = !m_controlPlayerMode;
		std::string hudText = Stringf( "Player control" );
		DebugAddMessage( hudText, 5.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 0, 0 ) );
	}


	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F2 ) )
	{
		m_currentMap->m_sunDirection.x -= 1.f;
		std::string hudText = Stringf( "Sun direction: %5.2f, %5.2f, %5.2f", m_currentMap->m_sunDirection.x, m_currentMap->m_sunDirection.y, m_currentMap->m_sunDirection.z );
		DebugAddMessage( hudText, 5.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 0, 0 ) );
	}

	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F3 ) )
	{
		m_currentMap->m_sunDirection.x += 1.f;
		std::string hudText = Stringf( "Sun direction: %5.2f, %5.2f, %5.2f", m_currentMap->m_sunDirection.x, m_currentMap->m_sunDirection.y, m_currentMap->m_sunDirection.z );
		DebugAddMessage( hudText, 5.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 0, 0 ) );
	}

	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F4 ) )
	{
		m_currentMap->m_sunDirection.y -= 1.f;
		std::string hudText = Stringf( "Sun direction: %5.2f, %5.2f, %5.2f", m_currentMap->m_sunDirection.x, m_currentMap->m_sunDirection.y, m_currentMap->m_sunDirection.z );
		DebugAddMessage( hudText, 5.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 0, 0 ) );
	}

	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F5 ) )
	{
		m_currentMap->m_sunDirection.y += 1.f;
		std::string hudText = Stringf( "Sun direction: %5.2f, %5.2f, %5.2f", m_currentMap->m_sunDirection.x, m_currentMap->m_sunDirection.y, m_currentMap->m_sunDirection.z );
		DebugAddMessage( hudText, 5.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 0, 0 ) );
	}

	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F6 ) && ( m_currentMap->m_sunIntensity >= .0f ) )
	{
		m_currentMap->m_sunIntensity -= 0.05f;
		std::string hudText = Stringf( "Sun intensity: %5.2f", m_currentMap->m_sunIntensity );
		DebugAddMessage( hudText, 5.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 0, 0 ) );
	}

	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F7 ) && ( m_currentMap->m_sunIntensity <= 1.0f ) )
	{
		m_currentMap->m_sunIntensity += 0.05f;
		std::string hudText = Stringf( "Sun intensity: %5.2f", m_currentMap->m_sunIntensity );
		DebugAddMessage( hudText, 5.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 0, 0 ) );
	}

	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F8 ) && ( m_currentMap->m_AmbientIntensity >= .0f ) )
	{
		m_currentMap->m_AmbientIntensity -= 0.05f;
		std::string hudText = Stringf( "Sun intensity: %5.2f", m_currentMap->m_AmbientIntensity );
		DebugAddMessage( hudText, 5.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 0, 0 ) );
	}

	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F9 ) && ( m_currentMap->m_AmbientIntensity <= 1.0f ) )
	{
		m_currentMap->m_AmbientIntensity += 0.05f;
		std::string hudText = Stringf( "Sun intensity: %5.2f", m_currentMap->m_AmbientIntensity );
		DebugAddMessage( hudText, 5.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 0, 0 ) );
	}

	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F11 ) )
	{
		m_currentMap = m_maps[( m_currentMapNumber + 1 ) % m_maps.size()];
		m_currentMapNumber = ( m_currentMapNumber + 1 ) % m_maps.size();
		std::string hudText = Stringf( "Swapped to map: %i", m_currentMapNumber );
		DebugAddMessage( hudText, 5.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 0, 0 ) );
	}

	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_LEFT_MOUSE ) )
	{
		Vec3 rayStart = m_player->m_position;
		Vec3 rayDir = m_player->m_orientation.GetAsMatrix_IFwd_JLeft_KUp().GetIBasis3D().GetNormalized();
		float rayDist = 10.f;

		RaycastResult3D result = m_currentMap->RaycastAll( rayStart, rayDir, rayDist );
		Mat44 toWorld = m_player->GetModelToWorldTransform();
		Vec3 endPos = m_player->m_position + toWorld.GetIBasis3D() * 10.f;
		DebugAddWorldCylinder( m_player->m_position, endPos, 0.01f, 10.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 255, 255 ), DebugRenderMode::X_RAY );
		if ( result.m_didImpact )
		{
			DebugAddWorldSphere( result.m_impactPos, 0.03f, 10.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 255, 255 ) );
			Vec3 arrowEndPos = result.m_impactPos + result.m_impactNormal * 0.2f;
			DebugAddWorldArrow( result.m_impactPos, arrowEndPos, 0.02f, 10.f, Rgba8( 0, 0, 255 ), Rgba8( 0, 0, 255 ) );
		}
	}


}

//-----------------------------------------------------------------------------------------------
void Game::RenderUI() const
{
	float screenSizeY = g_gameConfig->GetValue( "screenSizeY", 0.f );
	float screenSizeX = g_gameConfig->GetValue( "screenSizeX", 0.f );
	float fps = 1.f / ( float )g_engine->m_systemClock->GetDeltaSeconds();
	float scale = (float)g_engine->m_systemClock->GetTimeScale();
	std::string hudText = Stringf( "Time: %.2f FPS: %6.1f Scale: %.2f", m_roundTime, fps, scale );
	DebugAddScreenText( hudText, AABB2( Vec2( 0.f, screenSizeY - 25.f ), Vec2( screenSizeX, screenSizeY ) ), 15.f, Vec2( 1.f, 0.5f ), 0.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 255, 255 ) );

	hudText = Stringf( "Player position: %5.2f, %5.2f, %5.2f", m_player->m_position.x, m_player->m_position.y, m_player->m_position.z );
	DebugAddScreenText( hudText, AABB2( Vec2( 0.f, screenSizeY - 25.f ), Vec2( screenSizeX, screenSizeY ) ), 10.f, Vec2( 0.f, 0.5f ), 0.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 255, 255 ) );

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
	char title[32] = "Doomenstein";
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

}

void Game::ConstructMapFromXML()
{
	m_maps.clear();
	XmlDocument doc;
	XmlError eResult = doc.LoadFile( "Data/Definitions/MapDefinitions.xml" );
	if ( eResult == 0 )
	{
		XmlElement* rootElement = doc.RootElement();
		if ( rootElement )
		{
			XmlElement* mapDefElement = rootElement->FirstChildElement( "MapDefinition" );
			while ( mapDefElement )
			{
				IntVec2 dimensions = m_xml.ParseXmlAttribute( *mapDefElement, "dimensions", IntVec2( 25, 25 ) );
				std::string name = m_xml.ParseXmlAttribute( *mapDefElement, "name", "TestMap" );
				std::string imagePath = m_xml.ParseXmlAttribute( *mapDefElement, "image", "Data/Maps/TestMap.png" );
				std::string shaderPath = m_xml.ParseXmlAttribute( *mapDefElement, "shader", "Data/Shaders/Diffuse" );
				std::string texturePath = m_xml.ParseXmlAttribute( *mapDefElement, "spriteSheetTexture", "Data/Images/Terrain_8x8.png" );
				IntVec2 cellCount = m_xml.ParseXmlAttribute( *mapDefElement, "spriteSheetCellCount", IntVec2(0,0) );

				MapDefinition* mapDef = CreateMapDef( name, imagePath, shaderPath, texturePath, cellCount );
				m_maps.push_back( new Map( g_game, mapDef ) );

				mapDefElement = mapDefElement->NextSiblingElement( "MapDefinition" );
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
MapDefinition* Game::CreateMapDef( std::string name, std::string imagePath, std::string  shaderPath, std::string  texturePath, IntVec2 cellCount )
{
	MapDefinition* mapDef = new MapDefinition;
	mapDef->m_name = name;
	mapDef->m_image = Image( imagePath.c_str() );
	mapDef->m_shader = g_engine->m_render->CreateOrGetShader( shaderPath.c_str(), VertexType::VERTEX_PCUTBN );
	mapDef->m_spriteSheetCellCount = cellCount;
	mapDef->m_spriteSheetTexture =  g_engine->m_render->CreateOrGetTextureFromFile( texturePath.c_str() );

	return mapDef;
}