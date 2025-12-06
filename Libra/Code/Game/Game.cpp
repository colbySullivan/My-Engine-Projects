#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Game/Map.hpp"
#include "Game/Player.hpp"
#include "Game/Scorpio.hpp"
#include "Game/Tile.hpp"
#include <cmath>

RandomNumberGenerator g_rng;
XmlUtils m_xml;
Game* g_game = nullptr;

//-----------------------------------------------------------------------------------------------
Game::Game()
{
	g_game = this;
	m_worldCamera = new Camera;
	m_screenCamera = new Camera;
	m_roundNumber = 1;
	LoadSounds();
	InitializePauseVerts();
	InitializeWinLoseVerts();
	m_lobbyPlaybackID = g_engine->m_audio->StartSound( 0 );
	TileDefinition::InitializeTileDefs();
	LoadTextures();
}

//-----------------------------------------------------------------------------------------------
Game::~Game()
{
	if (m_currentMap)
	{
		delete m_currentMap;
		m_currentMap = nullptr;
	}

	delete g_game;
	delete g_engine;
	delete m_worldCamera;
	delete m_screenCamera;
	g_game = nullptr;
	g_engine = nullptr;
	m_worldCamera = nullptr;
	m_screenCamera = nullptr;
}

//-----------------------------------------------------------------------------------------------
// Game Loop
//-----------------------------------------------------------------------------------------------
void Game::Startup()
{
	m_endGameDelayTimer = 3.0f;
	ConstructMapFromXML();
	m_currentMap = m_maps[m_currentMapNumber];
	m_currentMap->SpawnNewEntity( ENTITY_TYPE_GOOD_PLAYER, Vec2( 1.5f, 1.5f ), 0.f, FACTION_GOOD );
	m_nextMap = m_currentMap;
	m_isPaused = false;
}

//-----------------------------------------------------------------------------------------------
void Game::Update(float deltaSeconds)
{
	m_frameTime += deltaSeconds;

	XboxController const& controller = g_engine->m_input->GetController( 0 );

	UpdateCameras( deltaSeconds );
	UpdateKeyboardInput( controller );

	if ( m_currentMap != m_nextMap )
	{
		MovePlayerToNewMap();
		m_currentMap = m_nextMap;
	}

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
		m_hasWon = false;
		m_isPaused = false;
		m_endGame = false;
		m_currentMapNumber = 0;
		UpdateAttractMode( deltaSeconds );
	}

	if ( m_currentGameState == GAMESTATE_PLAY )
	{
		if ( m_endGame )
		{
			m_endGameDelayTimer -= deltaSeconds;
			if ( m_endGameDelayTimer <= 0.f )
			{
				m_nextGameState = GAMESTATE_ATTRACT;
			}
		return;
		}
		PlayerPortalEndConditionCheck();
		if ( m_isSlowMo ) // T pressed
		{
			deltaSeconds = 1.f / 600.f; // Run at 1/10th the speed
		}

		if ( m_isFastMo ) // T pressed
		{
			deltaSeconds = deltaSeconds * 3; // Run at 1/10th the speed
		}

		if ( m_hasWon || m_currentMap->m_lostGame )
		{
			m_endGame = true;
			m_isPaused = true;
			return;
		}

		if ( !m_isPaused || m_pauseAfterNextUpdate )
		{
			m_pauseAfterNextUpdate = false; // Reset run token for simulation step
			g_engine->m_audio->SetSoundPlaybackSpeed(m_gameMusicPlaybackID, 1.0f);
			UpdateEntities( deltaSeconds );
		}
		else
		{
			g_engine->m_audio->SetSoundPlaybackSpeed(m_gameMusicPlaybackID, 0.0f);
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Game::Render() const
{
	Rgba8 backgroundColor = Rgba8(static_cast<unsigned char>(0.f), static_cast<unsigned char>(0.f), static_cast<unsigned char>(0.f), static_cast<unsigned char>(255.f)); // Suppresses error with conversion
	
	if ( m_currentGameState == GAMESTATE_ATTRACT )
	{
		g_engine->m_render->BindTexture( nullptr );
		RenderAttractMode();
		return;
	}

	if ( m_currentGameState == GAMESTATE_PLAY )
	{
		g_engine->m_render->ClearScreen(backgroundColor);
		g_engine->m_render->BeginCamera( *m_worldCamera );
		m_currentMap->Render();
		g_engine->m_render->EndCamera( *m_worldCamera );
		RenderUI();
		RenderEntities();
		if ( m_hasWon )
		{
			g_engine->m_audio->StartSound( 8, false, 0.8f ); // TODO fix repeat sounds
			RenderWinLoseSreen( m_endWinScreen );
		}
		if ( m_currentMap->m_lostGame )
		{
			g_engine->m_audio->StartSound( 9, false, 0.8f );
			RenderWinLoseSreen( m_endLoseScreen );
		}
		if ( m_isPaused && ( !m_currentMap->m_lostGame && !m_hasWon ))
		{
			RenderPauseSreen();
		}
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
	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_TILDA ))
	{
		g_engine->m_console->ToggleMode( DevConsoleMode::HIDDEN );
		if ( g_engine->m_console->GetMode() == DevConsoleMode::HIDDEN )
		{
			g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "Closed dev console", 20.f, m_frameTime );
		}
		else if ( g_engine->m_console->GetMode() == DevConsoleMode::OPEN_FULL )
		{
			g_engine->m_console->AddLine( DevConsole::INFO_MINOR_COLOR, "Opened dev console", 20.f, m_frameTime);
		}
	}

	if ( ( g_engine->m_input->WasKeyJustPressed( KEYCODE_ESC ) || controller.WasButtonJustPressed( XboxButtonID::BACK ) ) && m_currentGameState != GAMESTATE_ATTRACT && m_isPaused )
	{
		m_nextGameState = GAMESTATE_ATTRACT;
		m_lobbyPlaybackID = g_engine->m_audio->StartSound( 0 );
	}

	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_ESC ) || controller.WasButtonJustPressed( XboxButtonID::BACK ) )
	{
		if ( m_currentGameState == GAMESTATE_ATTRACT )
		{
			m_isQuitting = true;
			return;
		}

		if ( m_currentGameState == GAMESTATE_PLAY )
		{
			m_isPaused = true;
		}
	}

	m_isSlowMo = g_engine->m_input->IsKeyDown('T');  // Slows simulation time to 1/10th the normal rate

	m_isFastMo =  g_engine->m_input->IsKeyDown('X');

	if (g_engine->m_input->WasKeyJustPressed('P') || controller.WasButtonJustPressed(XboxButtonID::START)) // Pauses game
	{
		m_isPaused = !m_isPaused; // Switch pause
	}
	if (g_engine->m_input->WasKeyJustPressed('O')) // Runs a single paused Update (simulation step) and then pauses.
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
			m_gameMusicPlaybackID = g_engine->m_audio->StartSound( 2, false, 0.8f );
		}
	}

	if (g_engine->m_input->WasKeyJustPressed(KEYCODE_F9))
	{
		m_currentMapNumber += 1;
		if ( m_currentMapNumber < m_maps.size() )
		{
			m_nextMap = m_maps[m_currentMapNumber];
		}
		
		//RenderWinLoseSreen( m_endWinScreen );
	}

	if (g_engine->m_input->WasKeyJustPressed(KEYCODE_F1))
	{
		g_drawDebug = !g_drawDebug;
	}
	if ( m_currentMap )
	{
		m_currentMap->UpdatePlayerDevControls( controller );
	}

	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F6 ) )
	{
		m_mapRenderMode = static_cast<MapRenderMode>( ( m_mapRenderMode + 1 ) % NUM_MAP_MODES );
	}


	g_engine->m_input->EndFrame();
}

//-----------------------------------------------------------------------------------------------
void Game::RenderPauseSreen() const
{
	Vertex tempPauseWorldVerts[NUM_PLAYER_VERTS];

	for ( int vertIndex = 0; vertIndex < NUM_PLAYER_VERTS; ++vertIndex )
	{
		tempPauseWorldVerts[vertIndex] = m_pauseScreenVerts[vertIndex];
	}

	TransformVertexArrayXY3D( NUM_PLAYER_VERTS, tempPauseWorldVerts, 100.f, 0.0f, Vec2(0.0f, 0.0f));
	g_engine->m_render->DrawVertexArray( NUM_PLAYER_VERTS, tempPauseWorldVerts );
}

//-----------------------------------------------------------------------------------------------
void Game::RenderWinLoseSreen( Texture* texture ) const
{
	Vertex tempPauseWorldVerts[NUM_PLAYER_VERTS];

	for ( int vertIndex = 0; vertIndex < NUM_PLAYER_VERTS; ++vertIndex )
	{
		tempPauseWorldVerts[vertIndex] = m_winLoseScreen[vertIndex];
	}

	TransformVertexArrayXY3D( NUM_PLAYER_VERTS, tempPauseWorldVerts, 10.f, 0.0f, Vec2(WORLD_SIZE_X * 0.5f, WORLD_SIZE_Y * 0.5f));
	g_engine->m_render->BindTexture( texture );
	g_engine->m_render->DrawVertexArray( NUM_PLAYER_VERTS, tempPauseWorldVerts );
	g_engine->m_render->BindTexture( nullptr );
}

//-----------------------------------------------------------------------------------------------
void Game::RenderUI() const
{
	//Camera attractCamera;
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	g_engine->m_render->BeginCamera( *m_screenCamera );

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

	m_textOffset.x = std::fmod(m_textOffset.x + deltaSeconds, 1.f);
	if (m_textOffset.x < 0.f)
	{
		m_textOffset.x += 1.f;
	}
	m_textOffset.y = std::fmod(m_textOffset.y + deltaSeconds, 1.f);

	if (m_textOffset.y < 0.f)
	{
		m_textOffset.y += 1.f;
	}
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateEntities(float deltaSeconds)
{
	m_currentMap->Update( deltaSeconds );
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

	// Title
	/*char title[32] = "Libra";
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

	}*/

	std::vector<Vertex> verts;
	Vec2 mins( 0.0f, 0.0f );
	Vec2 maxs( SCREEN_SIZE_X, SCREEN_SIZE_Y );
	AABB2 localBox( mins, maxs );
	AddVertsForAABB2D( verts, localBox, Rgba8( 255, 255, 255, 255 ) );
	g_engine->m_render->BindTexture( m_startScreen );
	g_engine->m_render->DrawVertexArray( verts );
	g_engine->m_render->BindTexture( nullptr );

	std::vector<Vertex> explosionVerts;
	std::vector<Vertex> textVerts;
	std::vector<Vertex> textVerts2;
	std::vector<Vertex> boxOutlineVerts;
	std::vector<Vertex> boxOutlineVerts2;

	AddVertsForAABB2D( boxOutlineVerts, AABB2(Vec2(100.f, 250.f), Vec2(700.f, 350.f)), Rgba8( 0, 0, 0 ) );
	AddVertsForAABB2D( boxOutlineVerts2, AABB2(Vec2(300.f, 550.f), Vec2(800.f, 650.f)), Rgba8( 0, 0, 0 ) );
	g_testFont->AddVertsForTextInBox2D(textVerts, "Teemo\nis\nawesome?????", AABB2(Vec2(100.f, 250.f), Vec2(700.f, 350.f)), 40.f, Rgba8(100, 0, 0), 1.f, m_textOffset, TextBoxMode::SHRINK_TO_FIT);
	g_testFont->AddVertsForTextInBox2D(textVerts2, "Teemo\nis\nawesome?????", AABB2(Vec2(300.f, 550.f), Vec2(800.f, 650.f)), 50.f, Rgba8(100, 0, 0), 1.f, m_textOffset, TextBoxMode::OVERRUN);
	
	g_engine->m_render->DrawVertexArray( boxOutlineVerts );
	g_engine->m_render->DrawVertexArray( boxOutlineVerts2 );

	std::vector<Vertex> tileVerts;

	const SpriteDefinition& explosionSprite = m_tilesSpriteSheetAnim->GetSpriteDefAtTime( m_frameTime );
	Vec2 explosionMins, explosionMaxs;
	explosionSprite.GetUVs( explosionMins, explosionMaxs );

	float minX = 400.0f;
	float minY = 400.0f;
	float maxX = minX + 100.0f;
	float maxY = minY + 100.0f;
	AABB2 box = AABB2( minX, minY, maxX, maxY );

	AddVertsForAABB2D( explosionVerts, box, Rgba8(255,255,255), explosionMins, explosionMaxs);
	
	g_engine->m_render->BindTexture( &m_explosionSpriteSheet->GetTexture() );
	g_engine->m_render->DrawVertexArray( explosionVerts );

	g_engine->m_render->BindTexture( nullptr );
	g_engine->m_render->BindTexture( &g_testFont->GetTexture() );
	g_engine->m_render->DrawVertexArray( textVerts );
	g_engine->m_render->DrawVertexArray( textVerts2 );

	g_engine->m_render->EndCamera( *m_screenCamera );
}

//------------------------------------------------------------------------------
void Game::RenderEntities() const
{
	g_engine->m_render->BeginCamera(*m_worldCamera);

	m_currentMap->Render();

	g_engine->m_render->EndCamera(*m_worldCamera);
}

//-----------------------------------------------------------------------------------------------
// Utility Functions
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
void Game::LoadSounds()
{
	m_lobbyPlaybackID = g_engine->m_audio->CreateOrGetSound( "Data/Audio/Tank!.mp3" );		//	SoundID = 0
	g_engine->m_audio->CreateOrGetSound("Data/Audio/Roundstarts/tragic.mp3");				//	SoundID = 1
	m_gameMusicPlaybackID = g_engine->m_audio->CreateOrGetSound("Data/Audio/lobby.mp3");	//	SoundID = 2
	m_shootSound = g_engine->m_audio->CreateOrGetSound("Data/Audio/PlayerShootNormal.ogg"); //	SoundID = 3
	m_enemyDied = g_engine->m_audio->CreateOrGetSound( "Data/Audio/EnemyDied.wav" );		//	SoundID = 4
	m_bulletBounce = g_engine->m_audio->CreateOrGetSound( "Data/Audio/BulletRicochet.wav" );	//	SoundID = 5
	m_enemyHit = g_engine->m_audio->CreateOrGetSound( "Data/Audio/EnemyHit.wav" );			//	SoundID = 6
	m_playerHit = g_engine->m_audio->CreateOrGetSound( "Data/Audio/PlayerHit.wav" );		//	SoundID = 7
	m_victorySound = g_engine->m_audio->CreateOrGetSound( "Data/Audio/Victory.mp3" );		//	SoundID = 8
	m_lossSound = g_engine->m_audio->CreateOrGetSound( "Data/Audio/Victory.mp3" );			//	SoundID = 9

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

//-----------------------------------------------------------------------------------------------
void Game::InitializePauseVerts()
{
	m_pauseScreenVerts[0].m_position = Vec3( -0.5f, -0.5f, 0.f );
	m_pauseScreenVerts[1].m_position = Vec3( 0.5f, -0.5f, 0.f );
	m_pauseScreenVerts[2].m_position = Vec3( 0.5f, 0.5f, 0.f );

	m_pauseScreenVerts[3].m_position = Vec3( -0.5f, -0.5f, 0.f );
	m_pauseScreenVerts[4].m_position = Vec3( -0.5f, 0.5f, 0.f );
	m_pauseScreenVerts[5].m_position = Vec3( 0.5f, 0.5f, 0.f );

	for ( int vertIndex = 0; vertIndex < NUM_TURRET_VERTS; ++vertIndex )
	{
		m_pauseScreenVerts[vertIndex].m_color = Rgba8( 0, 0, 0, 100 );
	}
}

//-----------------------------------------------------------------------------------------------
void Game::InitializeWinLoseVerts()
{
	m_winLoseScreen[0] = Vertex( Vec3( -1.0f, -0.5f, 0.f ), Rgba8( 255, 255, 255, 255 ), Vec2( 0.f, 0.f ) );
	m_winLoseScreen[1] = Vertex( Vec3( 1.0f, -0.5f, 0.f ), Rgba8( 255, 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_winLoseScreen[2] = Vertex( Vec3( 1.0f, 0.5f, 0.f ), Rgba8( 255, 255, 255, 255 ), Vec2( 1.f, 1.f ) );
	m_winLoseScreen[3] = Vertex( Vec3( -1.0f, -0.5f, 0.f ), Rgba8( 255, 255, 255, 255 ), Vec2( 0.f, 0.f ) );
	m_winLoseScreen[4] = Vertex( Vec3( 1.0f, 0.5f, 0.f ), Rgba8( 255, 255, 255, 255 ), Vec2( 1.f, 1.f ) );
	m_winLoseScreen[5] = Vertex( Vec3( -1.0f, 0.5f, 0.f ), Rgba8( 255, 255, 255, 255 ), Vec2( 0.f, 1.f ) );
}

//-----------------------------------------------------------------------------------------------
MapDef Game::CreateMapDef( IntVec2 dimensions, std::string fillTile, std::string edgeTile, std::string sprinkleTile1, std::string sprinkleTile2, std::string barrierTile )
{
	MapDef mapDef;

	mapDef.m_dimensions = dimensions;
	mapDef.m_fillTileType = fillTile;
	mapDef.m_edgeTileType = edgeTile;
	mapDef.m_sprinkle1TileType = sprinkleTile1;
	mapDef.m_sprinkle2TileType = sprinkleTile2;
	mapDef.m_barrierTileType = barrierTile;

	return mapDef;
}

//-----------------------------------------------------------------------------------------------
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
				std::string wormTile;
				std::string fillTileType = m_xml.ParseXmlAttribute( *mapDefElement, "fileTileType", "LongGrass" );
				std::string edgeTileType = m_xml.ParseXmlAttribute( *mapDefElement, "edgeTileType", "LongGrass" );
				std::string worm1TileType = m_xml.ParseXmlAttribute( *mapDefElement, "worm1TileType", "LongGrass" );	/*worm1Count = "15"	worm1MaxLength = "12"*/
				std::string worm2TileType = m_xml.ParseXmlAttribute( *mapDefElement, "worm2TileType", "LongGrass" );	/*worm2Count = "60"	worm2MaxLength = "8"*/
				std::string startFloorTileType = m_xml.ParseXmlAttribute( *mapDefElement, "startFloorTileType", "LongGrass" );
				std::string startBunkerTileType = m_xml.ParseXmlAttribute( *mapDefElement, "startBunkerTileType", "LongGrass" );
				std::string endFloorTileType = m_xml.ParseXmlAttribute( *mapDefElement, "endFloorTileType", "LongGrass" );
				std::string endBunkerTileType = m_xml.ParseXmlAttribute( *mapDefElement, "endBunkerTileType", "LongGrass" );

				MapDef mapDef = CreateMapDef( dimensions, fillTileType, edgeTileType, worm1TileType, worm2TileType, startBunkerTileType );
				m_maps.push_back( new Map( g_game, mapDef ) );

				mapDefElement = mapDefElement->NextSiblingElement( "MapDefinition" );
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Game::PlayerPortalEndConditionCheck()
{
	if ( m_currentMap->IsPlayerOnPortal() )
	{
		if ( m_currentMapNumber == (m_maps.size() - 1) )
		{
			m_hasWon = true;
			m_isPaused = true;
			g_engine->m_audio->StopSound( m_gameMusicPlaybackID );
			return;
		}

		m_currentMapNumber = ( m_currentMapNumber + 1 ) % static_cast< int >( m_maps.size() );
		m_nextMap = m_maps[m_currentMapNumber];
		MovePlayerToNewMap();
		m_currentMap = m_nextMap;
	}
}

//-----------------------------------------------------------------------------------------------
void Game::MovePlayerToNewMap()
{
	Entity* player = m_currentMap->m_entityListsByType[ENTITY_TYPE_GOOD_PLAYER][0];
	player->m_position = Vec2( 1.5f, 1.5f );
	player->m_map = m_nextMap;
	m_currentMap->RemoveEntityFromMap( *player );
	m_nextMap->AddEntityToMap( *player );
}

//-----------------------------------------------------------------------------------------------
void Game::LoadTextures()
{
	Texture* spriteSheetTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/Terrain_8x8.png" );
	m_tilesSpriteSheet = new SpriteSheet( *spriteSheetTexture, IntVec2( 8, 8 ) );
	m_scorpioBodyTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/EnemyCannon.png" );
	m_playerBodyTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/PlayerTankBase.png" );
	m_playerTurretTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/PlayerTankTop.png" );
	m_scorpioBodyTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/EnemyTurretBase.png" );
	m_scorpioTurretTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/EnemyCannon.png" );
	m_leoBodyTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/EnemyTank4.png" );
	m_ariesBodyTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/EnemyAries.png" );
	m_goodBulletTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/FriendlyShell.png" );
	m_badBulletTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/EnemyBullet.png" );
	m_endWinScreen = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/VictoryScreen.jpg" );
	m_endLoseScreen = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/YouDiedScreen.png" );
	m_startScreen = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/AttractScreen.png" );
	g_testFont = g_engine->m_render->CreateOrGetBitmapFont( "Data/Fonts/SquirrelFixedFont" ); 
	Texture* spriteSheetTextureExplosion = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/Explosion_5x5.png" );
	m_explosionSpriteSheet = new SpriteSheet( *spriteSheetTextureExplosion, IntVec2( 5, 5 ) );
	m_tilesSpriteSheetAnim = new SpriteAnimDefinition( *m_explosionSpriteSheet, 0, 24, .05f, SpriteAnimPlaybackType::PINGPONG );
}
