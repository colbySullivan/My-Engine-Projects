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
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/UIButton2D.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Game/Map.hpp"
#include "Game/Player.hpp"
#include "Game/Tile.hpp"
#include "Game/App.hpp"
#include "Game/ActorDefinitions.hpp"
#include "Game/GameButtonDefinitions.hpp"
#include "Game/ItemDefinitions.hpp"
#include <cmath>

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
	InitializeButtonsAndEvents();
	//m_lobbyPlaybackID = g_engine->m_audio->StartSound( 0 );
	m_gameClock = new Clock( *g_engine->m_systemClock );
	ActorDefinitions::InitializeActorDefs();
	TileDefinition::InitializeTileDefs();
	SpriteAnimationDefinition::InitializeSpriteAnimationDefs();
	ItemDefinitions::InitializeItemDefs();
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
	m_currentMap->SpawnNewEntity( ENTITY_TYPE_GOOD_PLAYER, Vec2( m_currentMap->m_dimensions.x * 0.5f, m_currentMap->m_dimensions.y * 0.5f ), 0.f, FACTION_GOOD );
	m_nextMap = m_currentMap;
	m_isPaused = false;
}

//-----------------------------------------------------------------------------------------------
void Game::Update(float deltaSeconds)
{
	m_frameTime += deltaSeconds;

	XboxController const& controller = g_engine->m_input->GetController( 0 );

	UpdateKeyboardInput( controller );
	UpdateMousePosition();
	UpdateUIButtons();

	if ( m_currentMap != m_nextMap )
	{
		MovePlayerToNewMap();
		m_currentMap = m_nextMap;
	}

	if ( m_currentGameState != m_nextGameState )
	{
		if ( m_nextGameState == GAMESTATE_PLAY )
		{
			Startup();
		}

		if ( m_nextGameState == GAMESTATE_ITEM )
		{
			InitializeShopCards();
		}

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
	}

	if ( m_currentGameState == GAMESTATE_CHARACTER_SELECT )
	{
		UpdateCharacterSelectMode( deltaSeconds );
	}

	if ( m_currentGameState == GAMESTATE_ITEM )
	{
		UpdateItemMode( deltaSeconds );
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
			//g_engine->m_audio->SetSoundPlaybackSpeed(m_gameMusicPlaybackID, 1.0f);
			UpdateEntities( deltaSeconds );
		}
		else
		{
			//g_engine->m_audio->SetSoundPlaybackSpeed(m_gameMusicPlaybackID, 0.0f);
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Game::Render() const
{
	Rgba8 backgroundColor = Rgba8(static_cast<unsigned char>(0.f), static_cast<unsigned char>(0.f), static_cast<unsigned char>(0.f), static_cast<unsigned char>(255.f)); // Suppresses error with conversion
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	g_engine->m_render->ClearScreen( backgroundColor );
	RenderUI();

	if ( m_currentGameState == GAMESTATE_ATTRACT )
	{
		g_engine->m_render->BindTexture( nullptr );
		RenderAttractMode();
	}

	if ( m_currentGameState == GAMESTATE_CHARACTER_SELECT )
	{
		RenderCharacterSelectMode();
	}

	if ( m_currentGameState == GAMESTATE_ITEM )
	{
		RenderItemMode();
	}

	if ( m_currentGameState == GAMESTATE_PLAY )
	{
		//g_engine->m_render->ClearScreen(backgroundColor);
		g_engine->m_render->BeginCamera( *m_worldCamera );
		m_currentMap->Render();
		g_engine->m_render->EndCamera( *m_worldCamera );
		RenderUI();
		RenderEntities();
		if ( m_hasWon )
		{
			//g_engine->m_audio->StartSound( 8, false, 0.8f ); // TODO fix repeat sounds
			RenderWinLoseSreen( m_endWinScreen );
		}
		if ( m_currentMap->m_lostGame )
		{
			//g_engine->m_audio->StartSound( 9, false, 0.8f );
			RenderWinLoseSreen( m_endLoseScreen );
		}
		if ( m_isPaused && ( !m_currentMap->m_lostGame && !m_hasWon ))
		{
			RenderPauseSreen();
		}
	}
	RenderUIButtons();

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
		//m_lobbyPlaybackID = g_engine->m_audio->StartSound( 0 );
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
			//g_engine->m_audio->StopSound( m_lobbyPlaybackID );
			//m_gameMusicPlaybackID = g_engine->m_audio->StartSound( 2, false, 0.8f );
		}
	}

	if (g_engine->m_input->WasKeyJustPressed(KEYCODE_F9))
	{
		m_currentMapNumber += 1;
		if ( m_currentMapNumber < static_cast<int>(m_maps.size()) )
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
void Game::UpdateUIButtons()
{
	bool mousePressed = g_engine->m_input->IsKeyDown( KEYCODE_LEFT_MOUSE );
	// New version
	for ( auto button : GameButtonDefinitions::s_definitions )
	{
		if ( button.buttonRef && button.gameState == m_currentGameState )
		{
			button.buttonRef->Update( m_mouseScreenWindowPosition, mousePressed);
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Game::RenderUI() const
{
	float screenSizeY = g_gameConfig->GetValue( "screenSizeY", 800.f );
	float screenSizeX = g_gameConfig->GetValue( "screenSizeX", 1600.f );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( screenSizeX, screenSizeY ) );

	float fps = 1.f / ( float )g_engine->m_systemClock->GetDeltaSeconds();
	float scale = ( float )g_engine->m_systemClock->GetTimeScale();
	float totalTime = ( float )g_engine->m_systemClock->GetTotalSeconds();
	std::string hudText = Stringf( "Time: %.2f FPS: %6.1f Scale: %.2f", totalTime, fps, scale );
	DebugAddScreenText( hudText, AABB2( Vec2( 0.f, 0.f ), Vec2( screenSizeX, screenSizeY ) ), 15.f, Vec2( 1.f, 1.f ), 0.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 255, 255 ) );
}

//-----------------------------------------------------------------------------------------------
void Game::RenderUIButtons() const
{
	g_engine->m_render->BeginCamera( *m_screenCamera );

	for ( auto button : GameButtonDefinitions::s_definitions )
	{
		if ( button.buttonRef && button.gameState == m_currentGameState )
		{
			button.buttonRef->Render();
		}
	}
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
// Attract Mode
//-----------------------------------------------------------------------------------------------
void Game::UpdateEntities(float deltaSeconds)
{
	m_currentMap->Update( deltaSeconds );
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateCharacterSelectMode( float deltaSeconds )
{
	
}

//------------------------------------------------------------------------------
void Game::UpdateItemMode( float deltaSeconds )
{
	bool mousePressed = g_engine->m_input->IsKeyDown( KEYCODE_LEFT_MOUSE );

	for ( ShopItemCard* card : m_shopCards )
	{
		card->Update( m_mouseScreenWindowPosition, mousePressed );
	}
}

//-----------------------------------------------------------------------------------------------
void Game::RenderAttractMode() const
{
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );
	g_engine->m_render->BeginCamera( *m_screenCamera );

	std::vector<Vertex> verts;
	Vec2 mins( 0.0f, 0.0f );
	Vec2 maxs( SCREEN_SIZE_X, SCREEN_SIZE_Y );
	AABB2 localBox( mins, maxs );
	AddVertsForAABB2D( verts, localBox, Rgba8( 255, 255, 255, 255 ) );
	//g_engine->m_render->BindTexture( m_startScreen );
	//g_engine->m_render->DrawVertexArray( verts );
	g_engine->m_render->BindTexture( nullptr );

	g_engine->m_render->EndCamera( *m_screenCamera );
}

//-----------------------------------------------------------------------------------------------
void Game::RenderCharacterSelectMode() const
{

}

//------------------------------------------------------------------------------
void Game::RenderItemMode() const
{
	g_engine->m_render->BeginCamera( *m_screenCamera );

	for ( ShopItemCard* card : m_shopCards )
	{
		card->Render();
	}

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
	//m_lobbyPlaybackID = g_engine->m_audio->CreateOrGetSound( "Data/Audio/Tank!.mp3" );		//	SoundID = 0
	//g_engine->m_audio->CreateOrGetSound("Data/Audio/Roundstarts/tragic.mp3");				//	SoundID = 1
	//m_gameMusicPlaybackID = g_engine->m_audio->CreateOrGetSound("Data/Audio/lobby.mp3");	//	SoundID = 2
	//m_shootSound = g_engine->m_audio->CreateOrGetSound("Data/Audio/PlayerShootNormal.ogg"); //	SoundID = 3
	//m_enemyDied = g_engine->m_audio->CreateOrGetSound( "Data/Audio/EnemyDied.wav" );		//	SoundID = 4
	//m_bulletBounce = g_engine->m_audio->CreateOrGetSound( "Data/Audio/BulletRicochet.wav" );	//	SoundID = 5
	//m_enemyHit = g_engine->m_audio->CreateOrGetSound( "Data/Audio/EnemyHit.wav" );			//	SoundID = 6
	//m_playerHit = g_engine->m_audio->CreateOrGetSound( "Data/Audio/PlayerHit.wav" );		//	SoundID = 7
	//m_victorySound = g_engine->m_audio->CreateOrGetSound( "Data/Audio/Victory.mp3" );		//	SoundID = 8
	//m_lossSound = g_engine->m_audio->CreateOrGetSound( "Data/Audio/Victory.mp3" );			//	SoundID = 9

}

//-----------------------------------------------------------------------------------------------
void Game::InitializePauseVerts()
{
	AABB2 box = AABB2( Vec2( -0.5, -0.5 ), Vec2( 0.5, 0.5 ));
	AddVertsForAABB2D( m_pauseScreenVerts, box, Rgba8( 0, 0, 0, 100 ) );
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
MapDef Game::CreateMapDef( IntVec2 dimensions, std::string fillTile, std::string edgeTile, float hordeTimer, float bigBoyTimer, float difficultyMultiplier )
{
	MapDef mapDef;

	mapDef.m_dimensions = dimensions;
	mapDef.m_fillTileType = fillTile;
	mapDef.m_edgeTileType = edgeTile;
	mapDef.m_hordeTimer = hordeTimer;
	mapDef.m_bigBoyTimer = bigBoyTimer;
	mapDef.m_difficultyMultiplier = difficultyMultiplier;

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
				float hordeTimer = m_xml.ParseXmlAttribute( *mapDefElement, "hordeTimer", 2.0f );
				float bigBoyTimer = m_xml.ParseXmlAttribute( *mapDefElement, "bigBoyTimer", 5.0f );
				float difficultyMultiplier = m_xml.ParseXmlAttribute( *mapDefElement, "difficultyMultiplier", 1.0f );

				MapDef mapDef = CreateMapDef( dimensions, fillTileType, edgeTileType, hordeTimer, bigBoyTimer, difficultyMultiplier );
				m_maps.push_back( new Map( g_game, mapDef ) );

				mapDefElement = mapDefElement->NextSiblingElement( "MapDefinition" );
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateMousePosition()
{
	Vec2 normalizedMousePos = g_engine->m_input->GetCursorNormalizedPosition();

	Vec2 cameraBottomLeft = m_screenCamera->GetOrthographicBottomLeft();
	Vec2 cameraTopRight = m_screenCamera->GetOrthographicTopRight();

	float worldX = cameraBottomLeft.x + ( normalizedMousePos.x * ( cameraTopRight.x - cameraBottomLeft.x ) );

	float worldY = cameraTopRight.y - ( normalizedMousePos.y * ( cameraTopRight.y - cameraBottomLeft.y ) );

	Vec2 mouseScreenPos = Vec2( worldX, worldY );

	m_mouseScreenWindowPosition = mouseScreenPos;

	cameraBottomLeft = m_worldCamera->GetOrthographicBottomLeft();
	cameraTopRight = m_worldCamera->GetOrthographicTopRight();

	worldX = cameraBottomLeft.x + ( normalizedMousePos.x * ( cameraTopRight.x - cameraBottomLeft.x ) );

	worldY = cameraTopRight.y - ( normalizedMousePos.y * ( cameraTopRight.y - cameraBottomLeft.y ) );

	Vec2 mouseWorldPos = Vec2( worldX, worldY );

	m_mouseWorldWindowPosition = mouseWorldPos;
}

//-----------------------------------------------------------------------------------------------
void Game::InitializeButtonsAndEvents()
{
	GameButtonDefinitions::InitializeButtonDefs();
	SubscribeEventCallbackFunction("StartGame", Game::AdvanceGameMode);
	SubscribeEventCallbackFunction("CharacterSelect", Game::CharacterSelect);
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
	m_playerTurretTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/TheWizard.png" );
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
	m_tilesSpriteSheetAnim = new SpriteAnimDefinition( *m_explosionSpriteSheet, 0, 24, .05f, SpriteAnimPlaybackType::ONCE );
	m_tilesSpriteSheetAnimPingPong = new SpriteAnimDefinition( *m_explosionSpriteSheet, 0, 24, .05f, SpriteAnimPlaybackType::PINGPONG);
}

//-----------------------------------------------------------------------------------------------
bool Game::AdvanceGameMode( [[maybe_unused]] EventArgs& args )
{
	g_app->m_game->m_nextGameState = ( Game_State )( ( g_app->m_game->m_currentGameState + 1 ) % NUM_GAMESTATES );
	return false;
}

//-----------------------------------------------------------------------------------------------
bool Game::CharacterSelect( [[maybe_unused]] EventArgs& args )
{
	int characterNum = args.GetValue( "CharacterOption", -1 );
	if ( characterNum == 1 )
	{
		g_app->m_game->m_playerDefChosen = "WizardBase";
		g_app->m_game->m_nextGameState = ( Game_State )( ( g_app->m_game->m_currentGameState + 1 ) % NUM_GAMESTATES );
	}

	if ( characterNum == 2 )
	{
		g_app->m_game->m_playerDefChosen = "WizardMelee";
		g_app->m_game->m_nextGameState = ( Game_State )( ( g_app->m_game->m_currentGameState + 1 ) % NUM_GAMESTATES );
	}

	if ( characterNum == 3 )
	{
		g_app->m_game->m_playerDefChosen = "WizardRanged";
		g_app->m_game->m_nextGameState = ( Game_State )( ( g_app->m_game->m_currentGameState + 1 ) % NUM_GAMESTATES );
	}
	return false;
}

//------------------------------------------------------------------------------
void Game::InitializeShopCards()
{
	for ( ShopItemCard* card : m_shopCards )
	{
		delete card;
	}

	m_shopCards.clear();

	float cardSizeX = 260.f;
	float cardSizeY = 350.f;
	float cardCenterY = 400.f;
	float cardStartingCenter = 280.f;
	float cardOffset = 300.f;

	int cardIndex = 0;
	for ( auto& [name, item] : ItemDefinitions::s_definitions )
	{
		if ( cardIndex >= 4 )
		{
			break;
		}

		m_shopCards.push_back( new ShopItemCard( &item, Vec2( cardStartingCenter + ( cardOffset * cardIndex ), cardCenterY), cardSizeX, cardSizeY));
		cardIndex++;
	}
}