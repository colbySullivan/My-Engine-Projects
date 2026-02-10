#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include <ThirdParty/stb/stb_image.h>

RandomNumberGenerator g_rng;

//-----------------------------------------------------------------------------------------------
Game::Game()
{
	m_worldCamera = new Camera;
	m_screenCamera = new Camera;

	m_worldCamera->SetOrthoView(Vec2(0.f, 0.f), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y));

	m_roundNumber = 1;
	LoadSounds();
	g_testFont = g_engine->m_render->CreateOrGetBitmapFont( "Data/Fonts/SquirrelFixedFont" );
	m_lobbyPlaybackID = g_engine->m_audio->StartSound( 0 );
}

//-----------------------------------------------------------------------------------------------
Game::~Game()
{
	delete g_engine;
	delete m_worldCamera;
	delete m_screenCamera;
	g_engine = nullptr;
	m_worldCamera = nullptr;
	m_screenCamera = nullptr;
}

//-----------------------------------------------------------------------------------------------
// Game Loop
//-----------------------------------------------------------------------------------------------
void Game::Startup()
{
	Vec2 worldCenter(WORLD_SIZE_X * 0.5f, WORLD_SIZE_Y * 0.5f);
	m_isPaused = false;
	//g_engine->m_render->CreateTextureFromImage("Data/Textures/Test_StbiFlippedAndOpenGL.png");
}

//-----------------------------------------------------------------------------------------------
void Game::Update(float deltaSeconds)
{
	XboxController const& controller = g_engine->m_input->GetController( 0 );

	UpdateCameras( deltaSeconds );
	UpdateKeyboardInput( controller );

	if ( m_currentGameState != m_nextGameState )
	{
		m_currentGameState = m_nextGameState;
	}

	if (m_soundDurationTimer > 0.f)
	{
		m_soundDurationTimer -= deltaSeconds;
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
	}
}

//-----------------------------------------------------------------------------------------------
void Game::Render() const
{
	g_engine->m_render->BeginCamera( *m_worldCamera );
	Rgba8 backgroundColor = Rgba8(static_cast<unsigned char>(0.f), static_cast<unsigned char>(255.f), static_cast<unsigned char>(0.f), static_cast<unsigned char>(255.f)); // Suppresses error with conversion
	g_engine->m_render->ClearScreen(backgroundColor);

	if ( m_currentGameState == GAMESTATE_ATTRACT )
	{
		RenderAttractMode();
	}

	if ( m_currentGameState == GAMESTATE_ATTRACT )
	{
		return;
	}

	if ( m_currentGameState == GAMESTATE_PLAY )
	{
		RenderUI();
	}
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
	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_TILDA ) )
	{
		g_engine->m_console->ToggleMode( DevConsoleMode::HIDDEN );
		if ( g_engine->m_console->GetMode() == DevConsoleMode::HIDDEN )
		{
			g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "Closed dev console", 20.f, 0.0f );
		}
		else if ( g_engine->m_console->GetMode() == DevConsoleMode::OPEN_FULL )
		{
			g_engine->m_console->AddLine( DevConsole::INFO_MINOR_COLOR, "Opened dev console", 20.f, 0.0f );
		}
	}
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
		m_lobbyPlaybackID = g_engine->m_audio->StartSound( 0 );
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
			g_engine->m_audio->StopSound( m_lobbyPlaybackID );
			//m_gameMusicPlaybackID = g_engine->m_audio->StartSound( 1, false, 0.8f );
		}
	}

	if (g_engine->m_input->WasKeyJustPressed('I'))
	{

	}

	if (g_engine->m_input->WasKeyJustPressed(KEYCODE_F1))
	{
		g_drawDebug = !g_drawDebug;
	}

	g_engine->m_input->EndFrame();
}

//-----------------------------------------------------------------------------------------------
void Game::RenderUI() const
{
	//Camera attractCamera;
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	g_engine->m_render->BeginCamera( *m_screenCamera );

	char textBuffer[64];
	snprintf(textBuffer, sizeof(textBuffer), "Time: %.2f", m_roundTime);
	RenderText(textBuffer, Vec2(700.f, 750.f), 20.f, Rgba8(50, 150, 255, 255));
	g_engine->m_render->EndCamera( *m_screenCamera );
}

//-----------------------------------------------------------------------------------------------
void Game::RenderText(const char text[] , Vec2 pos, float height, Rgba8 color) const
{
	std::vector<Vertex> textVerts;
	AddVertsForTextTriangles2D( textVerts, text, pos, height, color, 1.f );
	g_engine->m_render->DrawVertexArray( ( int )textVerts.size(), textVerts.data() );
}


//-----------------------------------------------------------------------------------------------
void Game::UpdateCameras( float deltaSeconds )
{
	// Random camera shake
	float shakeHorizontal = g_rng.RollRandomFloatInRange( -m_camShakeAmount, m_camShakeAmount );
	float shakeVertical = g_rng.RollRandomFloatInRange( -m_camShakeAmount, m_camShakeAmount );
	
	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X + shakeHorizontal, WORLD_SIZE_Y + shakeVertical ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	if ( m_camShakeAmount > 0 )
	{
		m_camShakeAmount -= deltaSeconds;
	}

}

//-----------------------------------------------------------------------------------------------
// Attract Mode
//-----------------------------------------------------------------------------------------------
void Game::UpdateAttractMode(float deltaSeconds)
{
	if ( m_gameMusicPlaybackID != MISSING_SOUND_ID )
	{
		g_engine->m_audio->StopSound( m_gameMusicPlaybackID );
	}

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
	char title[32] = "Protogame2D";
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

//-----------------------------------------------------------------------------------------------
void Game::LoadSounds()
{
	m_lobbyPlaybackID = g_engine->m_audio->CreateOrGetSound( "Data/Audio/LobbyMusic.mp3" ); //	SoundID = 0
	g_engine->m_audio->CreateOrGetSound("Data/Audio/Roundstarts/tragic.mp3");				//	SoundID = 1

}

//-----------------------------------------------------------------------------------------------
void Game::HandleSound(SoundPlaybackID soundID, SoundPriority priority, float soundDuration)
{
	if (soundID == MISSING_SOUND_ID)
		return;

	if (priority == PRIORITY_LOW)
	{
		if (m_shootSound != MISSING_SOUND_ID)
 			g_engine->m_audio->StopSound(m_shootSound);

 		m_shootSound = soundID;
		m_shotSoundDurationTimer = soundDuration;
		return;
	}

	if (m_currentSound != MISSING_SOUND_ID && m_soundDurationTimer > 0.f)
	{
		if (priority < m_currentSoundPriority)
		{
			g_engine->m_audio->StopSound(soundID);
			return;
		}
	}

	if (m_currentSound != MISSING_SOUND_ID)
		g_engine->m_audio->StopSound(m_currentSound);

	m_currentSound = soundID;
	m_currentSoundPriority = priority;
	m_soundDurationTimer = soundDuration;
}

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