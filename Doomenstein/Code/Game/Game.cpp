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
#include <ThirdParty/stb/stb_image.h>
#include "Game/TileDefinitions.hpp"
#include "Game/ActorDefinitions.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/Player.hpp"
#include "Game/WeaponDefinition.hpp"
#include "Game/SpriteAnimationGroupDefinition.hpp"
#include "Game/PowerUpDefinitions.hpp"

Game* g_game = nullptr;


//-----------------------------------------------------------------------------------------------
Game::Game()
{
	m_screenCamera = new Camera;
	g_game = this;

	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	m_roundNumber = 1;
	g_testFont = g_engine->m_render->CreateOrGetBitmapFont( "Data/Fonts/SquirrelFixedFont" );
	m_testTexture = g_engine->m_render->CreateTextureFromImage( "Data/Images/MainScreen.png" );
	//m_testTexture = g_engine->m_render->CreateTextureFromImage( "Data/Textures/TestUV.png" );
	//m_testTexture = g_engine->m_render->CreateTextureFromImage( "Data/Textures/gradient.png" );
	//m_testTexture = g_engine->m_render->CreateTextureFromImage( "Data/Textures/harshgradiant.png" );
	//m_riftShader = g_engine->m_render->CreateOrGetShader( "Data/Shaders/SummonersRiftShader", VertexType::VERTEX_PCUTBN);
	m_gameClock = new Clock( *g_engine->m_systemClock );
	Vec2 worldCenter( WORLD_SIZE_X * 0.5f, WORLD_SIZE_Y * 0.5f );
	SetUpCamera();
	PrintConsoleHelpCommands();
	TileDefinition::InitializeTileDefs();
	ActorDefinition::InitializeActorDefs();
	SpriteAnimationDefinition::InitializeSpriteAnimationDefs();
	WeaponDefinition::InitializeWeaponDefs();
	MapDefinition::InitializeMapDefs();
	PowerUpDefinition::InitializePowerUpDefs();
	GetAndStartSoundsFromConfig( "Data/GameConfig.xml" );
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

	if ( m_playerController1 )
	{
		delete m_playerController1;
		m_playerController1 = nullptr;
	}
	if ( m_playerController2 )
	{
		delete m_playerController2;
		m_playerController2 = nullptr;
	}
	delete g_engine;
	delete m_screenCamera;
	g_engine = nullptr;
	m_screenCamera = nullptr;
	delete m_worldCamera1;
	m_worldCamera1 = nullptr;
	delete m_worldCamera2;
	m_worldCamera2 = nullptr;
	delete m_gameClock;
	m_gameClock = nullptr;
}

//-----------------------------------------------------------------------------------------------
// Game Loop
//-----------------------------------------------------------------------------------------------
void Game::Startup()
{
	g_engine->m_audio->SetNumListeners( m_numActivePlayers );
	CreateMapsFromDef();
	m_isPaused = false;
	CreateProps();
	g_engine->m_audio->StopSound( m_mainMenuMusicPlaybackID );
	g_engine->m_audio->StartSound( m_buttonClickSoundID );
	m_gameMusicPlaybackID = g_engine->m_audio->StartSound( m_gameMusicID, true, m_musicVolume );
	if ( m_maps.size() > 0 )
	{
		m_currentMapNumber = 2;
		m_currentMap = m_maps[m_currentMapNumber];
	}

	//XboxController const& controller = g_engine->m_input->GetController( 1 );
	//bool wantMultiplayer = false;
	//if ( controller.GetControllerID() )
	//{
	//	wantMultiplayer = true;
	//}
	if ( m_wantMultiplayer )
	{
		SetUpMultiplayer();
	}
	else
	{
		SetUpSinglePlayer();
	}

	if ( m_currentMap )
	{
		m_currentMap->Startup();
	}
	//m_teemoModel = LoadOBJFromFile( "Data/Textures/summoner_rift.obj", g_engine->m_render ); // The world is not ready for summoners rift obj
	//m_teemoModel = LoadOBJFromFile( "Data/Textures/Veigar.obj", g_engine->m_render );
	//m_teemoModel = LoadOBJFromFile( "Data/Textures/Summoner's Rift.obj", g_engine->m_render );
	//m_teemoModel = LoadOBJFromFile( "Data/Textures/Teemo.obj", g_engine->m_render );
	//m_teemoTexture = g_engine->m_render->CreateTextureFromImage( "Data/Textures/teemo_texture.png" );
	//m_teemoTexture = nullptr;
}

//-----------------------------------------------------------------------------------------------
void Game::Update()
{
	XboxController const& controller = g_engine->m_input->GetController( 0 );

	float deltaSeconds = ( float )m_gameClock->GetDeltaSeconds();


	if ( m_currentGameState != m_nextGameState )
	{
		m_currentGameState = m_nextGameState;
	}

	if ( m_currentGameState == GAMESTATE_ATTRACT )
	{
		if ( g_engine->m_audio->IsPlaying( m_gameMusicPlaybackID ) )
		{
			g_engine->m_audio->StopSound( m_gameMusicPlaybackID );
		}
		if ( !g_engine->m_audio->IsPlaying( m_mainMenuMusicPlaybackID ) )
		{
			m_mainMenuMusicPlaybackID = g_engine->m_audio->StartSound( ( m_mainMenuMusicID ), true, 0.1f );
		}
		UpdateAttractMode( deltaSeconds );
	}

	if ( m_currentGameState == GAMESTATE_PLAYER_CONNECT )
	{
		UpdatePlayerConnectMode( deltaSeconds );
	}

	if ( m_currentGameState == GAMESTATE_PLAY )
	{
		if ( g_engine->m_audio->IsPlaying( m_mainMenuMusicPlaybackID ) )
		{
			g_engine->m_audio->StopSound( m_mainMenuMusicPlaybackID );
		}
		if ( m_isSlowMo ) // T pressed
		{
			deltaSeconds = 1.f / 600.f; // Run at 1/10th the speed
		}

		if ( !m_isPaused || m_pauseAfterNextUpdate )
		{
			m_pauseAfterNextUpdate = false; // Reset run token for simulation step
		}
		m_roundTime += deltaSeconds;
		if ( m_currentMap )
		{
			m_currentMap->Update();

			if ( m_playerController1 )
			{
				m_playerController1->Update( deltaSeconds );
			}

			if ( m_playerController2 && m_numActivePlayers == 2 )
			{
				m_playerController2->Update( deltaSeconds );
			}
		}
		DebugInput();
	}
	UpdateKeyboardInput( controller );

}

//-----------------------------------------------------------------------------------------------
void Game::Render() const
{
	if ( m_currentGameState == GAMESTATE_ATTRACT )
	{
		g_engine->m_render->BindTexture( nullptr );
		g_engine->m_render->BeginCamera( *m_worldCamera1 );
		g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;

		Rgba8 backgroundColor = Rgba8( 0, 255, 0, 255 );
		g_engine->m_render->ClearScreen( backgroundColor );
		RenderAttractMode();

		g_engine->m_render->EndCamera( *m_worldCamera1 );
		return;
	}

	if ( m_currentGameState == GAMESTATE_PLAYER_CONNECT )
	{
		g_engine->m_render->BindTexture( nullptr );
		//g_engine->m_render->BeginCamera( *m_worldCamera1 );
		g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;

		Rgba8 backgroundColor = Rgba8( 120, 120, 120, 255 );
		g_engine->m_render->ClearScreen( backgroundColor );
		RenderPlayerConnecMode();

		//g_engine->m_render->EndCamera( *m_worldCamera1 );
		return;
	}

	if ( m_currentGameState == GAMESTATE_PLAY )
	{
		Rgba8 backgroundColor = Rgba8( 0, 0, 0, 0 );
		g_engine->m_render->ClearScreen( backgroundColor );

		if ( m_numActivePlayers == 2 )
		{
			RenderSplitScreen();
		}
		else
		{
			RenderSinglePlayer();
		}
	}

	g_engine->m_render->SetBlendMode( BlendMode::OPAQUE );
	g_engine->m_render->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
	g_engine->m_render->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
}

//-----------------------------------------------------------------------------------------------
void Game::Shutdown()
{
	for (int mapIndex = 0; mapIndex < m_maps.size() ; ++mapIndex)
	{
		delete m_maps[mapIndex];
		m_maps[mapIndex] = nullptr;
	}
	m_maps.clear();
	m_currentMap = nullptr;
}

//-----------------------------------------------------------------------------------------------
// Input Handling
//-----------------------------------------------------------------------------------------------
void Game::UpdateKeyboardInput( XboxController const& controller )
{
	bool buttonPressed = false;
	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_ESC ) || controller.WasButtonJustPressed( XboxButtonID::BACK ) )
	{
		buttonPressed = true;
		if ( m_currentGameState == GAMESTATE_ATTRACT )
		{
			m_isQuitting = true;
			return;
		}
	}

	if ( ( g_engine->m_input->WasKeyJustPressed( KEYCODE_ESC ) || controller.WasButtonJustPressed( XboxButtonID::BACK ) ) && m_currentGameState != GAMESTATE_ATTRACT && m_currentGameState != GAMESTATE_PLAYER_CONNECT )
	{
		buttonPressed = true;
		m_nextGameState = GAMESTATE_ATTRACT;
		Shutdown();
	}

	m_isSlowMo = g_engine->m_input->IsKeyDown('T');  // Slows simulation time to 1/10th the normal rate

	if (g_engine->m_input->WasKeyJustPressed('P') || controller.WasButtonJustPressed(XboxButtonID::START)) // Pauses game
	{
		m_isPaused = !m_isPaused; // Switch pause
	}
	if (g_engine->m_input->WasKeyJustPressed('O')) // Runs a single unpaused Update (simulation step) and then pauses.
	{
		buttonPressed = true;
		m_isPaused = true;
		m_pauseAfterNextUpdate = true; // Consumed to false after one run of update
	}
	if ( g_engine->m_input->WasKeyJustPressed( 'N' ) || controller.WasButtonJustPressed( XboxButtonID::START ) || g_engine->m_input->WasKeyJustPressed( ' ' )  )
	{
		buttonPressed = true;
		if ( m_currentGameState == GAMESTATE_ATTRACT )
		{
			m_usedButtonMethod = controller.WasButtonJustPressed( XboxButtonID::START ) ? START : SPACE;
			m_nextGameState = GAMESTATE_PLAYER_CONNECT;
		}

		//else if ( m_currentGameState == GAMESTATE_PLAYER_CONNECT && ( m_usedButtonMethod == SPACE ? controller.WasButtonJustPressed( XboxButtonID::START ) : g_engine->m_input->WasKeyJustPressed( ' ' ) ) )
		//{
		//	m_nextGameState = GAMESTATE_PLAY;
		//	Startup();
		//}
	}

	if (g_engine->m_input->WasKeyJustPressed(KEYCODE_F1))
	{
		g_drawDebug = !g_drawDebug;
	}

	if ( buttonPressed )
	{
		g_engine->m_audio->StartSound( m_buttonClickSoundID );
	}

	g_engine->m_input->EndFrame();
}

//------------------------------------------------------------------------------
void Game::DebugInput()
{
	/*if ( g_engine->m_input->WasKeyJustPressed( '1' ) )
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
	}*/


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
		m_currentMap->Shutdown();
		m_currentMap = m_maps[( m_currentMapNumber + 1 ) % m_maps.size()];
		m_currentMapNumber = ( m_currentMapNumber + 1 ) % m_maps.size();
		std::string hudText = Stringf( "Swapped to map: %i", m_currentMapNumber );
		DebugAddMessage( hudText, 5.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 0, 0 ) );
		m_currentMap->Startup();
	}

	/*if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_LEFT_MOUSE ) )
	{
		float rayDist = 10.f;
		Vec3 rayStart = GetRaycastOrigin();
		Vec3 rayDir = GetRaycastDirection();
		Mat44 toWorld = GetRaycastTransform();
		Actor* ownerActor = nullptr;
		if ( m_playerController && !m_playerController->m_isFreeFlyMode )
		{
			ownerActor = m_playerController->GetActor();
		}
		RaycastResult3D result = m_currentMap->RaycastAll( rayStart, rayDir, rayDist, ownerActor );

		Vec3 endPos = rayStart + rayDir * rayDist;
		DebugAddWorldCylinder( rayStart, endPos, 0.01f, 10.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 255, 255 ), DebugRenderMode::X_RAY );

		if ( result.m_didImpact )
		{
			DebugAddWorldSphere( result.m_impactPos, 0.06f, 10.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 255, 255 ) );
			Vec3 arrowEndPos = result.m_impactPos + result.m_impactNormal * 0.3f;
			DebugAddWorldArrow( result.m_impactPos, arrowEndPos, 0.03f, 10.f, Rgba8( 0, 0, 255 ), Rgba8( 0, 0, 255 ) );
		}
	}

	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_RIGHT_MOUSE ) )
	{
		float rayDist = 0.25f;
		Vec3 rayStart = GetRaycastOrigin();
		Vec3 rayDir = GetRaycastDirection();
		Mat44 toWorld = GetRaycastTransform();
		Actor* ownerActor = nullptr;
		if ( m_playerController && !m_playerController->m_isFreeFlyMode ) 
		{
			ownerActor = m_playerController->GetActor();
		}
		RaycastResult3D result = m_currentMap->RaycastAll( rayStart, rayDir, rayDist, ownerActor );

		Vec3 endPos = rayStart + rayDir * rayDist;
		DebugAddWorldCylinder( rayStart, endPos, 0.01f, 10.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 255, 255 ), DebugRenderMode::X_RAY );

		if ( result.m_didImpact )
		{
			DebugAddWorldSphere( result.m_impactPos, 0.06f, 10.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 255, 255 ) );
			Vec3 arrowEndPos = result.m_impactPos + result.m_impactNormal * 0.3f;
			DebugAddWorldArrow( result.m_impactPos, arrowEndPos, 0.03f, 10.f, Rgba8( 0, 0, 255 ), Rgba8( 0, 0, 255 ) );
		}
	}*/


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

	m_wantMultiplayer = false;

	m_shipAnimationTimer += deltaSeconds;

	if ( m_shipAnimationTimer > SHIP_ANIMATION_DURATION )
	{
		m_shipAnimationTimer = 0.0f;
	}
	//UpdateBlackHole();
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

	std::vector<Vertex> hudVerts;
	AddVertsForAABB2D( hudVerts, AABB2( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) ), Rgba8( 255, 255, 255 ) );
	g_engine->m_render->BindTexture( m_testTexture );
	g_engine->m_render->DrawVertexArray( ( int )hudVerts.size(), hudVerts.data() );
	g_engine->m_render->BindTexture( nullptr );
	// Title
	char title[64] = "Doomenstein Zombies";
	for ( int charIndex = 0; charIndex < 20; ++charIndex )
	{
		char singleChar[2] = { title[charIndex], '\0' };
		int offsetColorR =  g_rng->RollRandomIntInRange(200, 255);
		int offsetColorG =  g_rng->RollRandomIntInRange(200, 255);
		int offsetColorB =  g_rng->RollRandomIntInRange(0, 255);
		RenderText( singleChar, Vec2( 475.f + charIndex * 40.f, 100.f ), 40.0f, Rgba8( static_cast<unsigned char>(offsetColorR), static_cast<unsigned char>(offsetColorG), static_cast<unsigned char>(offsetColorB) ) );

	}
	char nameTitle[96] = "Made by Colby Sullivan";
	for ( int charIndex = 0; charIndex < 22; ++charIndex )
	{
		char singleChar[2] = { nameTitle[charIndex], '\0' };
		int offsetColorR = g_rng->RollRandomIntInRange( 200, 255 );
		int offsetColorG = g_rng->RollRandomIntInRange( 200, 255 );
		int offsetColorB = g_rng->RollRandomIntInRange( 0, 255 );
		RenderText( singleChar, Vec2( 390.f + charIndex * 40.f, 50.f ), 40.0f, Rgba8( static_cast< unsigned char >( offsetColorR ), static_cast< unsigned char >( offsetColorG ), static_cast< unsigned char >( offsetColorB ) ) );

	}

	std::vector<Vertex> testTextureVerts;
	AABB2 texturedAABB2( 0.f, 0.f, 512.f, 512.f );
	AddVertsForAABB2D( testTextureVerts, texturedAABB2, Rgba8( 255, 255, 255, 255 ) );

	g_engine->m_render->EndCamera( *m_screenCamera );
}

//-----------------------------------------------------------------------------------------------
void Game::UpdatePlayerConnectMode( [[maybe_unused]] float deltaSeconds )
{
	XboxController const& controller = g_engine->m_input->GetController( 0 );

	bool startBtn = controller.WasButtonJustPressed( XboxButtonID::START );
	bool backBtn = controller.WasButtonJustPressed( XboxButtonID::BACK );
	bool spaceKey = g_engine->m_input->WasKeyJustPressed( ' ' );
	bool escKey = g_engine->m_input->WasKeyJustPressed( KEYCODE_ESC );

	bool p1Start = spaceKey;
	bool p1Back = escKey;
	bool p2Start = startBtn;
	bool p2Back = backBtn;

	if ( m_usedButtonMethod == START )
	{
		p1Start = startBtn;
		p1Back = backBtn;

		p2Start = spaceKey;
		p2Back = escKey;
	}

	if ( !m_wantMultiplayer )
	{
		if ( p2Start )
		{
			m_wantMultiplayer = true;
			return;
		}
		else if ( p1Start )
		{
			m_nextGameState = GAMESTATE_PLAY;
			Startup();
		}
		else if ( p1Back )
		{
			m_nextGameState = GAMESTATE_ATTRACT;
		}
	}
	else
	{
		if ( p1Start )
		{ 
			m_nextGameState = GAMESTATE_PLAY;
			Startup();
		}
		else if ( p2Back )
		{
			m_wantMultiplayer = false;
		}
		else if ( p1Back )
		{
			m_usedButtonMethod = ( m_usedButtonMethod == START ) ? SPACE : START;
			m_wantMultiplayer = false;
		}
	}
	g_engine->m_input->EndFrame();

}

//-----------------------------------------------------------------------------------------------
void Game::RenderPlayerConnecMode() const
{
	float screenSizeY = g_gameConfig->GetValue( "screenSizeY", 0.f );
	float screenSizeX = g_gameConfig->GetValue( "screenSizeX", 0.f );

	std::string p1StartBtn = ( m_usedButtonMethod == START ) ? "START" : "SPACE";
	std::string p1BackBtn = ( m_usedButtonMethod == START ) ? "BACK" : "ESCAPE";

	std::string p2StartBtn = ( m_usedButtonMethod == START ) ? "SPACE" : "START";
	std::string p2BackBtn = ( m_usedButtonMethod == START ) ? "ESCAPE" : "BACK";

	std::string playerText = Stringf( "Player 1 (Joined)" );
	DebugAddScreenText( playerText, AABB2( Vec2( 0.f, 0.f ), Vec2( screenSizeX, screenSizeY ) ), 30.f, Vec2( 0.5f, m_wantMultiplayer ? 0.8f : 0.5f ), 0.f, Rgba8( 0, 255, 0 ), Rgba8( 0, 255, 0 ) );

	std::string connectionInfoText;
	if ( !m_wantMultiplayer )
	{
		connectionInfoText = Stringf( "Press %s to CONFIRM AND START game\nPress %s to leave game\nPress %s to join player 2", p1StartBtn.c_str(), p1BackBtn.c_str(), p2StartBtn.c_str() );
	}
	else
	{
		connectionInfoText = Stringf( "Press %s to CONFIRM AND START game\nPress %s to leave game", p1StartBtn.c_str(), p1BackBtn.c_str() );
	}
	DebugAddScreenText( connectionInfoText, AABB2( Vec2( 0.f, -100.f ), Vec2( screenSizeX, screenSizeY ) ), 15.f, Vec2( 0.5f, m_wantMultiplayer ? 0.7f : 0.5f ), 0.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 255, 255 ) );

	if ( m_wantMultiplayer )
	{
		playerText = Stringf( "Player 2 (Joined)" );
		DebugAddScreenText( playerText, AABB2( Vec2( 0.f, 0.f ), Vec2( screenSizeX, screenSizeY ) ), 30.f, Vec2( 0.5f, 0.3f ), 0.f, Rgba8( 0, 255, 0 ), Rgba8( 0, 255, 0 ) );

		connectionInfoText = Stringf( "Press %s to CONFIRM AND START game\nPress %s to leave game", p2StartBtn.c_str(), p2BackBtn.c_str() );
		DebugAddScreenText( connectionInfoText, AABB2( Vec2( 0.f, -50.f ), Vec2( screenSizeX, screenSizeY ) ), 15.f, Vec2( 0.5f, 0.3f ), 0.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 255, 255 ) );
	}

	DebugRenderScreen( *m_screenCamera );
}

//-----------------------------------------------------------------------------------------------
// Utility Functions
//-----------------------------------------------------------------------------------------------

void Game::UpdateBlackHole()
{
	float holeRadii[NUM_BLACK_HOLE_SIDES] = {};
	for ( int sideNum = 0; sideNum < NUM_BLACK_HOLE_SIDES; ++sideNum )
	{
		holeRadii[sideNum] = g_rng->RollRandomFloatInRange( 1.9f, 2.0f );
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

//-----------------------------------------------------------------------------------------------
void Game::SetUpCamera()
{
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( g_gameConfig->GetValue( "screenSizeX", 0.f ), g_gameConfig->GetValue( "screenSizeY", 0.f ) ) );
	m_worldCamera1 = new Camera;
	float aspect = ( ( float )g_engine->m_window->GetClientDimensions().x / ( float )g_engine->m_window->GetClientDimensions().y );
	m_worldCamera1->SetPerspectiveView( aspect, 60.f, 0.1f, 100.f );
	Mat44 cameraToRenderMatrix;
	cameraToRenderMatrix.SetIJK3D( Vec3( 0.f, 0.f, 1.f ), Vec3( -1.f, 0.f, 0.f ), Vec3( 0.f, 1.f, 0.f ) );
	m_worldCamera1->SetCameraToRenderTransform( cameraToRenderMatrix );
}

//-----------------------------------------------------------------------------------------------
void Game::PrintConsoleHelpCommands()
{
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "=== CAMERA CONTROLS ===" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "W / Left Stick Up			: Move forward" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "S / Left Stick Down		: Move backward" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "A / Left Stick Left		: Move left" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "D / Left Stick Right		: Move right" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "Left Mouse / Right Trigger : Shoot" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "C / Right Shoulder			: Move up" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "Shift / A-Button			: Sprint" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "H / Start					: Reset position and orientation" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "P							: Pause / unpause game clock" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "1 / X                      : Weapon Slot 1" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "2 / Y                      : Weapon Slot 2" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "================================" );
}

//------------------------------------------------------------------------------
void Game::SetUpMultiplayer()
{
	m_numActivePlayers = 2;

	XboxController const& controller1 = g_engine->m_input->GetController( 0 );
	XboxController const& controller2 = g_engine->m_input->GetController( 1 );
	if ( controller1.IsConnected() && controller2.IsConnected() )
	{
		m_numOfControllers = 2;
	}
	else if ( controller1.IsConnected() || controller2.IsConnected() )
	{
		m_numOfControllers = 1;
	}

	float screenWidth = g_gameConfig->GetValue( "screenSizeX", 1600.f );
	float screenHeight = g_gameConfig->GetValue( "screenSizeY", 900.f );
	float halfHeight = screenHeight * 0.5f;
	float aspectRatio = screenWidth / halfHeight;

	Mat44 cameraToRenderMatrix;
	cameraToRenderMatrix.SetIJK3D( Vec3( 0.f, 0.f, 1.f ), Vec3( -1.f, 0.f, 0.f ), Vec3( 0.f, 1.f, 0.f ) );

	//m_worldCamera1 = new Camera();
	m_worldCamera1->SetViewport( AABB2( 0.0f, 0.5f, 1.0f, 1.0f ) );
	m_worldCamera1->SetPerspectiveView( aspectRatio, 60.0f, 0.1f, 100.0f );
	m_worldCamera1->SetCameraToRenderTransform( cameraToRenderMatrix );

	m_worldCamera2 = new Camera();
	m_worldCamera2->SetCameraToRenderTransform( cameraToRenderMatrix );
	m_worldCamera2->SetViewport( AABB2( 0.0f, 0.0f, 1.0f, 0.5f ) );
	m_worldCamera2->SetPerspectiveView( aspectRatio, 60.0f, 0.1f, 100.0f );
	m_worldCamera2->SetCameraToRenderTransform( cameraToRenderMatrix );

	m_playerController1 = new PlayerController( m_currentMap, m_worldCamera1 );
	m_playerController1->SetControllerIndex( 1, m_numOfControllers );

	m_playerController2 = new PlayerController( m_currentMap, m_worldCamera2 );
	m_playerController2->SetControllerIndex( 0, m_numOfControllers );
}

//-----------------------------------------------------------------------------------------------
void Game::SetUpSinglePlayer()
{
	m_numActivePlayers = 1;
	XboxController const& controller1 = g_engine->m_input->GetController( 0 );
	if ( controller1.IsConnected() )
	{
		m_numOfControllers = 1;
	}
	else
	{
		m_numOfControllers = 0;
	}

	//m_worldCamera1 = new Camera();
	m_worldCamera1->SetViewport( AABB2( 0.0f, 0.0f, 1.0f, 1.0f ) );

	float screenWidth = g_gameConfig->GetValue( "screenSizeX", 1600.f );
	float screenHeight = g_gameConfig->GetValue( "screenSizeY", 900.f );
	float aspectRatio = screenWidth / screenHeight;

	m_worldCamera1->SetPerspectiveView( aspectRatio, 60.0f, 0.1f, 100.0f );

	Mat44 cameraToRenderMatrix;
	cameraToRenderMatrix.SetIJK3D( Vec3( 0.f, 0.f, 1.f ), Vec3( -1.f, 0.f, 0.f ), Vec3( 0.f, 1.f, 0.f ) );
	m_worldCamera1->SetCameraToRenderTransform( cameraToRenderMatrix );

	m_playerController1 = new PlayerController( m_currentMap, m_worldCamera1 );
	m_playerController1->SetControllerIndex( 0, -1 ); // -1 insures player gets controller input
}

//-----------------------------------------------------------------------------------------------
void Game::CreateMapsFromDef()
{
	for ( auto& pair : MapDefinition::s_definitions )
	{
		MapDefinition* def = pair.second;
		m_maps.push_back( new Map( g_game, def ) );
	}
}
//-----------------------------------------------------------------------------------------------
Vec3 Game::GetRaycastOrigin( PlayerController* playerController ) const
{
	if ( playerController && !playerController->m_isFreeFlyMode )
	{
		Actor* actor = playerController->GetActor();
		if ( actor && actor->m_actorDef )
		{
			Vec3 rayOrigin = actor->m_position;
			rayOrigin.z += actor->m_actorDef->m_eyeHeight;
			return rayOrigin;
		}
	}
	else if ( playerController )
	{
		return playerController->GetCameraPosition();
	}
	return m_player ? m_player->m_position : Vec3( 0, 0, 0 );
}

//-----------------------------------------------------------------------------------------------
Vec3 Game::GetRaycastDirection() const
{
	if ( m_playerController1 && !m_playerController1->m_isFreeFlyMode )
	{
		Actor* actor = m_playerController1->GetActor();
		if ( actor )
		{
			return m_playerController1->GetRaycastDirection();
		}
	}
	else if ( m_playerController1 )
	{
		return m_playerController1->GetRaycastDirection();
	}
	return Vec3( 1.f, 0.f, 0.f );
}

//-----------------------------------------------------------------------------------------------
Mat44 Game::GetRaycastTransform() const
{
	if ( m_playerController1 && !m_playerController1->m_isFreeFlyMode )
	{
		Actor* actor = m_playerController1->GetActor();
		if ( actor )
		{
			return actor->GetModelToWorldTransform();
		}
	}
	return Mat44();
}

//------------------------------------------------------------------------------
void Game::RenderSplitScreen() const
{
	g_engine->m_render->SetBlendMode( BlendMode::OPAQUE );
	g_engine->m_render->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
	g_engine->m_render->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );

	g_engine->m_render->BindTexture( nullptr );
	g_engine->m_render->BeginCamera( *m_worldCamera1 );

	if ( m_currentMap )
	{
		m_currentMap->Render( m_worldCamera1 );
	}

	g_engine->m_render->EndCamera( *m_worldCamera1 );
	DebugRenderWorld( *m_worldCamera1 );

	if ( m_playerController1 )
	{
		m_playerController1->RenderUI();
	}

	g_engine->m_render->SetBlendMode( BlendMode::OPAQUE );
	g_engine->m_render->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
	g_engine->m_render->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );

	g_engine->m_render->BindTexture( nullptr );
	g_engine->m_render->BeginCamera( *m_worldCamera2 );

	if ( m_currentMap )
	{
		m_currentMap->Render( m_worldCamera2 );
	}

	g_engine->m_render->EndCamera( *m_worldCamera2 );
	DebugRenderWorld( *m_worldCamera2 );

	if ( m_playerController2 )
	{
		m_playerController2->RenderUI();
	}
}

//------------------------------------------------------------------------------
void Game::RenderSinglePlayer() const
{
	g_engine->m_render->BindTexture( nullptr );
	g_engine->m_render->BeginCamera( *m_worldCamera1 );
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;

	if ( m_currentMap )
	{
		m_currentMap->Render( m_worldCamera1 );
	}

	g_engine->m_render->EndCamera( *m_worldCamera1 );
	DebugRenderWorld( *m_worldCamera1 );

	if ( m_playerController1 )
	{
		m_playerController1->RenderUI();
	}

	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_NONE;

	if ( m_playerController1 )
	{
		Actor* playerActor = m_playerController1->GetActor();
		if ( playerActor && playerActor->m_isDead )
		{
			std::vector<Vertex> overlayVerts;
			AABB2 screenBounds( 0.f, 0.f, SCREEN_SIZE_X, SCREEN_SIZE_Y );
			AddVertsForAABB2D( overlayVerts, screenBounds, Rgba8( 128, 128, 128, 128 ) );

			g_engine->m_render->BindTexture( nullptr );
			g_engine->m_render->DrawVertexArray( ( int )overlayVerts.size(), overlayVerts.data() );
		}
	}

	g_engine->m_render->EndCamera( *m_worldCamera1 );

	DebugRenderWorld( *m_worldCamera1 );
	RenderUI();

	/*Vec3 modelPosition = Vec3( 50.0f, 50.0f, -0.1f );
	float modelScale = 0.1f;
	float modelRotation = 90.f;       */

	//g_engine->m_render->BindShader( m_riftShader );
	//Mat44 modelMatrix;
	//modelMatrix.SetTranslation3D( modelPosition );
	//modelMatrix.AppendScaleUniform3D( modelScale );
	//modelMatrix.AppendXRotation( modelRotation );
	//g_engine->m_render->SetModelConstants( modelMatrix, Rgba8( 120, 120, 120, 255 ) );
	//g_engine->m_render->BindTexture( m_testTexture );
	//g_engine->m_render->DrawIndexBuffer( m_teemoModel->m_vbo, m_teemoModel->m_ibo, m_teemoModel->m_indexCount );
}

//-----------------------------------------------------------------------------------------------
void Game::GetAndStartSoundsFromConfig( char const* filePath )
{
	XmlDocument doc;
	doc.LoadFile( filePath );

	XmlElement* root = doc.RootElement();
	if ( !root )
	{
		return;
	}

	//XmlElement* gameElem = root->FirstChildElement( "GameConfig" );
	XmlElement* gameElem = root;
	XmlUtils xml;

	while ( gameElem )
	{
		std::string mainMenuMusicPath = xml.ParseXmlAttribute( *gameElem, "mainMenuMusic", "" );
		std::string gameMusicPath = xml.ParseXmlAttribute( *gameElem, "gameMusic", "" );
		std::string buttonClickSoundPath = xml.ParseXmlAttribute( *gameElem, "buttonClickSound", "" );
		m_musicVolume = xml.ParseXmlAttribute( *gameElem, "musicVolume", 1.f );

		m_mainMenuMusicID = g_engine->m_audio->CreateOrGetSound( mainMenuMusicPath );
		m_gameMusicID = g_engine->m_audio->CreateOrGetSound( gameMusicPath );
		m_buttonClickSoundID = g_engine->m_audio->CreateOrGetSound( buttonClickSoundPath );

		gameElem = gameElem->NextSiblingElement( "GameConfig" );
	}

	m_mainMenuMusicPlaybackID = g_engine->m_audio->StartSound( ( m_mainMenuMusicID ), true, m_musicVolume );
}
