#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/Bullet.hpp"
#include "Game/Asteroid.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"
#include "Game/Beetle.hpp"
#include "Game/Debris.hpp"
#include "Game/Entity.hpp"
#include "Game/Wasp.hpp"
#include "Game/Interactable.hpp"

//-----------------------------------------------------------------------------------------------
Game::Game()
{
	//m_gameCamera = new Camera;
	m_worldCamera = new Camera;
	m_screenCamera = new Camera;
	g_engine = new Engine;
	m_roundNumber = 1;
	LoadSounds();
	CreateBlackHole();
	GenerateStars();
	m_lobbyPlaybackID = g_engine->m_audio->StartSound( 6 );
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
	m_playerShip = new PlayerShip(this, worldCenter);
	m_roundNumber = 1;
	m_roundEndTimer = 3;
	m_roundTime = 0.f;
	UpdateWaves();
}

//-----------------------------------------------------------------------------------------------
void Game::Update(float deltaSeconds)
{
	XboxController const& controller = g_engine->m_input->GetController( 0 );

	if ( m_currentGameState != m_nextGameState )
	{
		m_currentGameState = m_nextGameState;
	}

	if ( m_isSlowMo ) // T pressed
		deltaSeconds = 1.f / 600.f; // Run at 1/10th the speed

	if(AttractModeExitEnter( deltaSeconds, controller ))
		return;

	if (m_soundDurationTimer > 0.f)
	{
		m_soundDurationTimer -= deltaSeconds;
	}
	RoundTimer( deltaSeconds );
	CreateBlackHole();
	UpdateCameras( deltaSeconds );
	KeyboardInput( deltaSeconds, controller );
	DestroyGarbageEntities();
}

//-----------------------------------------------------------------------------------------------
void Game::Render() const
{
	g_engine->m_render->BeginCamera(*m_worldCamera);
	Rgba8 backgroundColor = Rgba8(static_cast<unsigned char>(0.f), static_cast<unsigned char>(0.f), static_cast<unsigned char>(0.f), static_cast<unsigned char>(255.f)); // Suppresses error with conversion
	g_engine->m_render->ClearScreen(backgroundColor);
	g_engine->m_render->DrawVertexArray( NUM_STAR_VERTS, m_starVerts ); // Draw stars first so none overlay 
	RenderEntities();
}

//-----------------------------------------------------------------------------------------------
void Game::Shutdown()
{

}

//-----------------------------------------------------------------------------------------------
// Input Handling
//-----------------------------------------------------------------------------------------------
void Game::KeyboardInput( float deltaSeconds, XboxController const& controller )
{
	if ( ( g_engine->m_input->WasKeyJustPressed( KEYCODE_ESC ) || controller.WasButtonJustPressed( XboxButtonID::BACK ) ) && m_currentGameState != GAMESTATE_ATTRACT )
	{
		m_nextGameState = GAMESTATE_ATTRACT;
		m_lobbyPlaybackID = g_engine->m_audio->StartSound( 6 );
	}

	m_isSlowMo = g_engine->m_input->IsKeyDown('T');  // Slows simulation time to 1/10th the normal rate

	if (g_engine->m_input->WasKeyJustPressed('P')) // Pauses game
	{
		m_isPaused = !m_isPaused; // Switch pause
	}
	if (g_engine->m_input->WasKeyJustPressed('O')) // Runs a single unpaused Update (simulation step) and then pauses.
	{
		m_isPaused = true;
		m_pauseAfterNextUpdate = true; // Consumed to false after one run of update
	}

	if (g_engine->m_input->WasKeyJustPressed('I'))
	{
		//SpawnRandomAsteroids();
		SpawnRandomInteractable();
	}

	if (g_engine->m_input->WasKeyJustPressed(KEYCODE_F1))
	{
		g_drawDebug = !g_drawDebug;
	}

	//if ( m_roundNumber > 5 )
	//{
	//	m_nextGameState = GAMESTATE_ATTRACT;
	//}
		

	if ( m_playerShip->m_lives <= 0 && m_playerShip->m_isDead )
	{
		if ( !m_playingEndSound )
		{
			m_endPlaybackID = g_engine->m_audio->StartSound( 5 ); // End sound
			m_playingEndSound = true;
		}
		m_roundEndTimer -= deltaSeconds;
		if ( m_roundEndTimer <= 0 )
		{
			m_nextGameState = GAMESTATE_ATTRACT;
			g_engine->m_audio->StopSound( m_endPlaybackID ); // End sound
			m_lobbyPlaybackID = g_engine->m_audio->StartSound( 6 );
			m_playingEndSound = false;
		}
	}

	g_engine->m_input->EndFrame();
}

//-----------------------------------------------------------------------------------------------
// Entity Spawning
//-----------------------------------------------------------------------------------------------
Asteroid* Game::SpawnRandomAsteroids()
{
	for (int astroidIndex = 0; astroidIndex < MAX_ASTEROIDS; ++astroidIndex)
	{
		if (m_asteroid[astroidIndex] == nullptr)
		{
			m_asteroid[astroidIndex] = new Asteroid(this, Vec2(0,0)); //Position will be randomly chosen in asteroid class
			return m_asteroid[astroidIndex];
		}
	}
	ERROR_RECOVERABLE("Cannot spawn a new asteroid; all array slots are full");
	return nullptr;
}

//-----------------------------------------------------------------------------------------------
Bullet* Game::SpawnBullet(Vec2 const& pos, float forwardDegrees)
{
	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
	{
		// Bullet* bullet = m_bullets[bulletIndex];
		if (m_bullets[bulletIndex] == nullptr)
		{
			m_bullets[bulletIndex] = new Bullet(this, pos);
			m_bullets[bulletIndex]->m_orientationDegrees = forwardDegrees;
			m_bullets[bulletIndex]->m_velocity.x = BULLET_SPEED * CosDegrees(forwardDegrees);
			m_bullets[bulletIndex]->m_velocity.y = BULLET_SPEED * SinDegrees(forwardDegrees);
			return m_bullets[bulletIndex];
		}
	}

	ERROR_RECOVERABLE("Cannot spawn a new bullet; all slots are full");
	return nullptr;
}

//-----------------------------------------------------------------------------------------------
Beetle* Game::SpawnNewRandomBeetle()
{
	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
	{
		if (m_beetles[beetleIndex] == nullptr)
		{
			m_beetles[beetleIndex] = new Beetle(this, Vec2(0, 0)); //Position will be randomly chosen in astroid class
			return m_beetles[beetleIndex];
		}
	}
	ERROR_RECOVERABLE("Cannot spawn a new beetle; all array slots are full");
	return nullptr;
}

//-----------------------------------------------------------------------------------------------
Wasp* Game::SpawnNewRandomWasp()
{
	for (int waspIndex = 0; waspIndex < MAX_WASP; ++waspIndex)
	{
		if (m_wasp[waspIndex] == nullptr)
		{
			m_wasp[waspIndex] = new Wasp(this, Vec2(0, 0)); //Position will be randomly chosen in astroid class
			return m_wasp[waspIndex];
		}
	}
	ERROR_RECOVERABLE("Cannot spawn a new beetle; all array slots are full");
	return nullptr;
}

//------------------------------------------------------------------------------
Interactable* Game::SpawnRandomInteractable()
{
	for (int interactableIndex = 0; interactableIndex < MAX_INTERACTABLES; ++interactableIndex)
	{
		if (m_interactable[interactableIndex] == nullptr)
		{
			m_interactable[interactableIndex] = new Interactable(this, Vec2(100.f, 50.f));
			return m_interactable[interactableIndex];
		}
	}
	ERROR_RECOVERABLE("Cannot spawn a new interactable; all array slots are full");
	return nullptr;
}

//-----------------------------------------------------------------------------------------------
Debris* Game::SpawnNewDebris(Vec2 pos, Rgba8 color, Vec2 velocity, float size)
{
	for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++debrisIndex)
	{
		if (m_debris[debrisIndex] == nullptr)
		{
			m_debris[debrisIndex] = new Debris(this, pos);
			m_debris[debrisIndex]->m_entityColor = Rgba8(color.r, color.g, color.b, 127);
			m_debris[debrisIndex]->m_size = size;
			float heading = g_rng.RollRandomFloatInRange(0.f, 360.0f);
			float speed = g_rng.RollRandomFloatInRange(10.f, 100.f);
			Vec2 localVelocity = Vec2::MakeFromPolarDegrees( heading, speed );
			m_debris[debrisIndex]->m_velocity = (velocity + localVelocity) * 0.5f;

			return m_debris[debrisIndex];
		}
	}
	ERROR_RECOVERABLE("Cannot spawn new debris; all array slots are full");
	return nullptr;
}

//-----------------------------------------------------------------------------------------------
void Game::SpawnDebrisCluster(Vec2 pos, Rgba8 entityColor, Vec2 velocity, int debrisAmount, float size)
{
	for (int debrisIndex = 0; debrisIndex < debrisAmount; ++debrisIndex)
	{
		SpawnNewDebris(pos, entityColor, velocity, size);
	}
}

//-----------------------------------------------------------------------------------------------
// Entity Updates
//-----------------------------------------------------------------------------------------------
void Game::UpdateEntities(float deltaSeconds)
{
	m_playerShip->Update(deltaSeconds);

	for (int interactableIndex = 0; interactableIndex < MAX_INTERACTABLES; ++interactableIndex)
	{
		Interactable* interactable = m_interactable[interactableIndex];
		if (interactable)
		{
			interactable->Update(deltaSeconds);
		}
	}

	for (int astroidIndex = 0; astroidIndex < MAX_ASTEROIDS; ++astroidIndex)
	{
		Asteroid* astroid = m_asteroid[astroidIndex];
		if (astroid)
		{
			astroid->Update(deltaSeconds);
		}
	}

	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
	{
		Bullet* bullet = m_bullets[bulletIndex];
		if (bullet)
		{
			bullet->Update(deltaSeconds);
		}

	}

	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
	{
		Beetle* beetle = m_beetles[beetleIndex];
		if (beetle)
		{
			beetle->Update(deltaSeconds);
		}
	}

	for (int waspIndex = 0; waspIndex < MAX_WASP; ++waspIndex)
	{
		Wasp* wasp = m_wasp[waspIndex];
		if (wasp)
		{
			wasp->Update(deltaSeconds);
		}
	}

	for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++debrisIndex)
	{
		Debris* debris = m_debris[debrisIndex];
		if (debris)
		{
			debris->Update(deltaSeconds);
		}
	}

	CheckBeetlePush();
}

//------------------------------------------------------------------------------
bool Game::CheckInvincibilityMode()
{
	return m_playerShip->m_invincibilityTimer > 0.f;
}

//-----------------------------------------------------------------------------------------------
// Collision Detection
//-----------------------------------------------------------------------------------------------
void Game::CheckBulletsVsEnemies()
{
	for (int astroidIndex = 0; astroidIndex < MAX_ASTEROIDS; ++astroidIndex)
	{
		Asteroid* astroid = m_asteroid[astroidIndex];
		if (astroid)
		{
			for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
			{
				Bullet* bullet = m_bullets[bulletIndex];
				if (bullet)
				{
					CheckBulletsVsEnemies(*bullet, *astroid);
				}
			}
		}
	}

	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
	{
		Beetle* beetle = m_beetles[beetleIndex];
		if (beetle)
		{
			for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
			{
				Bullet* bullet = m_bullets[bulletIndex];
				if (bullet)
				{
					CheckBulletsVsEnemies(*bullet, *beetle);
				}
			}
		}
	}

	for (int waspIndex = 0; waspIndex < MAX_WASP; ++waspIndex)
	{
		Wasp* wasp = m_wasp[waspIndex];
		if (wasp)
		{
			for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
			{
				Bullet* bullet = m_bullets[bulletIndex];
				if (bullet)
				{
					CheckBulletsVsEnemies(*bullet, *wasp);
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Game::CheckBulletsVsEnemies(Bullet& bullet, Entity& enemy)
{
	if (DoDiscsOverlap(enemy.m_position, enemy.m_physicsRadius, bullet.m_position, bullet.m_physicsRadius))
	{
		bullet.Die();
		enemy.m_health -= 1;
		m_camShakeAmount = SHAKE_TRAUMA_AMOUNT / 2; //TODO
	}
}

//-----------------------------------------------------------------------------------------------
void Game::CheckEnemiesVsShips()
{
	for (int astroidIndex = 0; astroidIndex < MAX_ASTEROIDS; ++astroidIndex)
	{
		Asteroid* astroid = m_asteroid[astroidIndex];
		if (astroid && !m_playerShip->m_isDead)
		{
			CheckEnemiesVsShip(*astroid, *m_playerShip);
		}
	}

	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
	{
		Beetle* beetle = m_beetles[beetleIndex];
		if (beetle && !m_playerShip->m_isDead)
		{
			CheckEnemiesVsShip(*beetle, *m_playerShip);
		}
	}
	for (int waspIndex = 0; waspIndex < MAX_WASP; ++waspIndex)
	{
		Wasp* wasp = m_wasp[waspIndex];
		if (wasp && !m_playerShip->m_isDead)
		{
			CheckEnemiesVsShip(*wasp, *m_playerShip);
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Game::CheckEnemiesVsShip(Entity& enemy, PlayerShip& ship)
{
	if (DoDiscsOverlap(enemy.m_position, enemy.m_physicsRadius, ship.m_position, ship.m_physicsRadius))
	{
		enemy.m_health -= 1;
		m_camShakeAmount = SHAKE_TRAUMA_AMOUNT;

		if (CheckInvincibilityMode())
		{
			SoundPlaybackID temp = g_engine->m_audio->StartSound(7, false, 0.3f);
			HandleSound(temp, PRIORITY_MEDIUM, 0.2f);
			return;
		}
		ship.m_health -= 1;
	}
}

//-----------------------------------------------------------------------------------------------
void Game::CheckBeetlePush()
{
	for ( int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex )
	{
		Beetle* beetle = m_beetles[beetleIndex];
		if ( beetle )
		{
			for ( int otherBeetleIndex = beetleIndex + 1; otherBeetleIndex < MAX_BEETLES; ++otherBeetleIndex )
			{
				Beetle* otherBeetle = m_beetles[otherBeetleIndex];

				if ( otherBeetle )
					PushDiscsOutOfEachOther2D( beetle->m_position, beetle->m_cosmeticRadius, otherBeetle->m_position, otherBeetle->m_cosmeticRadius );
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
// Rendering
//-----------------------------------------------------------------------------------------------
void Game::RenderEntities() const
{
	for (int interactableIndex = 0; interactableIndex < MAX_INTERACTABLES; ++interactableIndex)
	{
		Interactable* interactable = m_interactable[interactableIndex];
		if (interactable)
		{
			interactable->Render();
		}
	}

	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
	{
		Bullet const* bullet = m_bullets[bulletIndex];
		if (bullet)
		{
			bullet->Render();
		}
	}
	for (int astroidIndex = 0; astroidIndex < MAX_ASTEROIDS; ++astroidIndex)
	{
		Asteroid const* astroid = m_asteroid[astroidIndex];
		if (astroid)
		{
			astroid->Render();
		}
	}

	if (m_playerShip)
	{
		m_playerShip->Render();
	}

	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
	{
		Beetle const* beetle = m_beetles[beetleIndex];
		if (beetle) 
		{
			beetle->Render();
		}
	}

	for (int waspIndex = 0; waspIndex < MAX_WASP; ++waspIndex)
	{
		Wasp* wasp = m_wasp[waspIndex];
		if (wasp)
		{
			wasp->Render();
		}
	}

	for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++debrisIndex)
	{
		Debris* debris = m_debris[debrisIndex];
		if (debris)
		{
			debris->Render();
		}
	}

}

//-----------------------------------------------------------------------------------------------
void Game::RenderUI()
{
	//Camera attractCamera;
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	g_engine->m_render->BeginCamera( *m_screenCamera );
	for (int shipLives = 1; shipLives < m_playerShip->m_lives; ++shipLives)
	{
		Vertex fakePlayerShipVerts[NUM_SHIP_VERTS];
		PlayerShip::InitializeLocalPlayerShipsVerts(fakePlayerShipVerts);
		TransformVertexArrayXY3D(NUM_SHIP_VERTS, fakePlayerShipVerts, 10.1f, 90.f, Vec2(shipLives * 50.0f - 20.f, 775.0f));
		g_engine->m_render->DrawVertexArray(NUM_SHIP_VERTS, fakePlayerShipVerts);
	}

	char textBuffer[64];
	snprintf(textBuffer, sizeof(textBuffer), "Time: %.2f", m_roundTime);
	RenderText(textBuffer, Vec2(700.f, 750.f), 20.f, Rgba8(50, 150, 255, 255));
	g_engine->m_render->EndCamera( *m_screenCamera );
}

//-----------------------------------------------------------------------------------------------
void Game::RenderText(const char text[] , Vec2 pos, float height, Rgba8 color)
{
	std::vector<Vertex> textVerts;
	AddVertsForTextTriangles2D( textVerts, text, pos, height, color, 1.f );
	g_engine->m_render->DrawVertexArray( ( int )textVerts.size(), textVerts.data() );
}

void Game::UpdateCameras( float deltaSeconds )
{
	// Random camera shake
	float shakeHorizontal = g_rng.RollRandomFloatInRange( -m_camShakeAmount, m_camShakeAmount );
	float shakeVertical = g_rng.RollRandomFloatInRange( -m_camShakeAmount, m_camShakeAmount );
	

	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X + shakeHorizontal, WORLD_SIZE_Y + shakeVertical ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	if ( m_camShakeAmount > 0 )
		m_camShakeAmount -= deltaSeconds;

}

//-----------------------------------------------------------------------------------------------
bool Game::AttractModeExitEnter( float deltaSeconds, XboxController const& controller )
{
	if ( m_currentGameState == GAMESTATE_ATTRACT )
	{
		if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_ESC ) || controller.WasButtonJustPressed( XboxButtonID::BACK ) )
		{
			m_isQuitting = true;
			return true;
		}

		if ( g_engine->m_input->WasKeyJustPressed( ' ' ) || g_engine->m_input->WasKeyJustPressed( 'N' ) || controller.WasButtonJustPressed( XboxButtonID::START ) )
		{
			m_nextGameState = GAMESTATE_PLAY;
			Startup();
			g_engine->m_audio->StopSound( m_lobbyPlaybackID );

		}
		else
		{
			CleanupGameEntities();
			UpdateAttractMode( deltaSeconds );
		}
	}

	if ( m_currentGameState == GAMESTATE_ATTRACT )
	{
		g_engine->m_input->EndFrame();
		return true;
	}

	RenderUI();

	if ( IsReadyToStartNextWave() )
	{
		m_roundNumber++;
		UpdateWaves();
	}

	if ( !m_isPaused || m_pauseAfterNextUpdate )
	{
		UpdateEntities( deltaSeconds );
		CheckBulletsVsEnemies();
		CheckEnemiesVsShips();
		m_pauseAfterNextUpdate = false; // Reset run token for simulation step
	}
	return false;
}

//-----------------------------------------------------------------------------------------------
// Attract Mode
//-----------------------------------------------------------------------------------------------
void Game::UpdateAttractMode(float deltaSeconds)
{
	m_alphaTimer += deltaSeconds;
	float normalizedTime = fmodf(m_alphaTimer, 3.0f);
	float alpha;

	if (normalizedTime <= 3.0f)
	{
		alpha = RangeMapClamped(normalizedTime, 0.0f, 1.5f, 60.0f, 255.0f);
	}
	else
	{
		alpha = RangeMapClamped(normalizedTime, 1.5f, 3.0f, 255.0f, 60.0f);
	}

	m_shipAnimationTimer += deltaSeconds;
	if ( m_shipAnimationTimer > SHIP_ANIMATION_DURATION )
	{
		m_shipAnimationTimer = 0.0f;
	}

	RenderAttractMode(alpha);
}

//-----------------------------------------------------------------------------------------------
void Game::RenderAttractMode( float playButtonAlpha )
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
	CreateBlackHole(); // randomize vertices
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

	// Player ships
	float animProgress = m_shipAnimationTimer / SHIP_ANIMATION_DURATION;
	for ( int renderedShipsLocationOffset = 0; renderedShipsLocationOffset < 2; ++renderedShipsLocationOffset )
	{
		Vertex fakePlayerShipVerts[NUM_SHIP_VERTS];
		PlayerShip::InitializeLocalPlayerShipsVerts( fakePlayerShipVerts );

		Vec2 currentPos = Vec2(
			Interpolate(200.0f + ( renderedShipsLocationOffset * 1200 ), SCREEN_SIZE_X / 2, animProgress),
			Interpolate(400.0f, SCREEN_SIZE_Y / 2, animProgress)
		);

		float currentScale = Interpolate(100.f, 0.f, animProgress);

		float currentRotation = Interpolate(360, 0.f, animProgress);

		TransformVertexArrayXY3D(
			NUM_SHIP_VERTS,
			fakePlayerShipVerts,
			currentScale,
			( currentRotation + ( renderedShipsLocationOffset * 180 ) ),
			currentPos );
		g_engine->m_render->DrawVertexArray( NUM_SHIP_VERTS, fakePlayerShipVerts );
	}


	// Play button
	Vertex playButton[3];
	playButton[0].m_position = Vec3( 0.f, 0.f, 0.f );
	playButton[1].m_position = Vec3( 0.f, 1.f, 0.f );
	playButton[2].m_position = Vec3( 1.f, 0.5f, 0.f );
	for ( int vertIndex = 0; vertIndex < 3; ++vertIndex )
	{
		playButton[vertIndex].m_color = Rgba8( 0, 255, 0, static_cast< unsigned char >( playButtonAlpha ) );
	}
	TransformVertexArrayXY3D(
		3,
		playButton,
		300.0f,
		0.f,
		Vec2( 650.f, 250.f ) );
	//g_engine->m_render->DrawVertexArray( 3, playButton );

	// Title
	char title[] = "Starship Gold";
	for ( int charIndex = 0; charIndex < 14; ++charIndex )
	{
		char singleChar[1] = { title[charIndex] };
		int offsetColorR =  g_rng.RollRandomIntInRange(200, 255);
		int offsetColorG =  g_rng.RollRandomIntInRange(200, 255);
		int offsetColorB =  g_rng.RollRandomIntInRange(0, 255);
		RenderText( singleChar, Vec2( 525.f + charIndex * 40.f, 100.f ), 40.0f, Rgba8( ( unsigned char )offsetColorR, ( unsigned char )offsetColorG, ( unsigned char )offsetColorB ) );

	}

	g_engine->m_render->EndCamera( *m_screenCamera );
}



//-----------------------------------------------------------------------------------------------
// Wave Management
//-----------------------------------------------------------------------------------------------
int Game::GetNumLivingEnemies() const
{
	int numLivingEnemies = 0;
	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
	{
		Beetle* beetle = m_beetles[beetleIndex];
		if (isAlive(beetle))
		{
			++numLivingEnemies;
		}
	}
	for (int waspIndex = 0; waspIndex < MAX_WASP; ++waspIndex)
	{
		Wasp* wasp = m_wasp[waspIndex];
		if (isAlive(wasp))
		{
			++numLivingEnemies;
		}
	}
	return numLivingEnemies;
}

//-----------------------------------------------------------------------------------------------
bool Game::IsReadyToStartNextWave() const
{
	int numLivingEnemies = GetNumLivingEnemies();
	return numLivingEnemies == 0 || m_spawnBuffer <= 0.f;
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateWaves()
{
	if (IsReadyToStartNextWave())
	{
		m_spawnBuffer = ROUND_BUFFER;
		for (int i = 0; i < NUM_STARTING_ASTEROIDS; ++i)
		{
			SpawnRandomAsteroids();
		}
		for (int i = 0; i < g_rng.RollRandomIntInRange(m_roundNumber, m_roundNumber+2); ++i)
			SpawnNewRandomBeetle();

		for (int i = 0; i < g_rng.RollRandomIntInRange(m_roundNumber, m_roundNumber+1); ++i)
			SpawnNewRandomWasp();

		//for (int i = 0; i < g_rng.RollRandomIntInRange(1, m_roundNumber); ++i)
		SpawnRandomInteractable();

	}
}

//-----------------------------------------------------------------------------------------------
// Entity Cleanup
//-----------------------------------------------------------------------------------------------
void Game::DestroyGarbageEntities()
{
	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
	{
		Bullet const* bullet = m_bullets[bulletIndex];
		if (bullet && bullet->m_isGarbage)
		{
			delete bullet;
			m_bullets[bulletIndex] = nullptr;
		}
	}
	for (int astroidIndex = 0; astroidIndex < MAX_ASTEROIDS; ++astroidIndex)
	{
		Asteroid const* astroid = m_asteroid[astroidIndex];
		if (astroid && astroid->m_isGarbage)
		{
			delete astroid;
			m_asteroid[astroidIndex] = nullptr;
		}
	}
	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
	{
		Beetle const* beetle = m_beetles[beetleIndex];
		if (beetle && beetle->m_isGarbage)
		{
			delete beetle;
			m_beetles[beetleIndex] = nullptr;
		}
	}
	for (int waspIndex = 0; waspIndex < MAX_WASP; ++waspIndex)
	{
		Wasp const* wasp = m_wasp[waspIndex];
		if (wasp && wasp->m_isGarbage)
		{
			delete wasp;
			m_wasp[waspIndex] = nullptr;
		}
	}
	for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++debrisIndex)
	{
		Debris const* debris = m_debris[debrisIndex];
		if (debris && debris->m_isGarbage)
		{
			delete debris;
			m_debris[debrisIndex] = nullptr;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Game::CleanupGameEntities()
{
	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
	{
		if (m_bullets[bulletIndex])
		{
			m_bullets[bulletIndex]->m_isDead = true;
			m_bullets[bulletIndex]->m_isGarbage = true;
		}
	}

	for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex)
	{
		if (m_asteroid[asteroidIndex])
		{
			m_asteroid[asteroidIndex]->m_isDead = true;
			m_asteroid[asteroidIndex]->m_isGarbage = true;
		}
	}

	for (int beetlesIndex = 0; beetlesIndex < MAX_BEETLES; ++beetlesIndex)
	{
		if (m_beetles[beetlesIndex])
		{
			m_beetles[beetlesIndex]->m_isDead = true;
			m_beetles[beetlesIndex]->m_isGarbage = true;
		}
	}

	for (int waspIndex = 0; waspIndex < MAX_WASP; ++waspIndex)
	{
		if (m_wasp[waspIndex])
		{
			m_wasp[waspIndex]->m_isDead = true;
			m_wasp[waspIndex]->m_isGarbage = true;
		}
	}

	for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++debrisIndex)
	{
		if (m_debris[debrisIndex])
		{
			m_debris[debrisIndex]->m_isDead = true;
			m_debris[debrisIndex]->m_isGarbage = true;
		}
	}

	if (m_playerShip)
	{
		Vec2 worldCenter(WORLD_SIZE_X * 0.5f, WORLD_SIZE_Y * 0.5f);
		m_playerShip->m_position = worldCenter;
		m_playerShip->m_isDead = true;
		m_playerShip->m_isGarbage = true;
	}

	DestroyGarbageEntities();
}

//-----------------------------------------------------------------------------------------------
// Utility Functions
//-----------------------------------------------------------------------------------------------
bool Game::isAlive(Entity* entity) const
{
	return (entity && !entity->m_isDead);
}

//-----------------------------------------------------------------------------------------------
void Game::LoadSounds()
{
	g_engine->m_audio->CreateOrGetSound( "Data/Audio/TestSound.mp3" ); //						SoundID = 0
	g_engine->m_audio->CreateOrGetSound( "Data/Audio/Laser_Shoot.wav" ); //						SoundID = 1
	g_engine->m_audio->CreateOrGetSound( "Data/Audio/Hit_Hurt.wav" ); //						SoundID = 2
	g_engine->m_audio->CreateOrGetSound( "Data/Audio/Respawn.wav" ); //							SoundID = 3
	g_engine->m_audio->CreateOrGetSound( "Data/Audio/Explosion.wav" ); //						SoundID = 4
	m_endPlaybackID = g_engine->m_audio->CreateOrGetSound( "Data/Audio/GameOver.mp3" ); //		SoundID = 5
	m_lobbyPlaybackID = g_engine->m_audio->CreateOrGetSound( "Data/Audio/LobbyMusic.mp3" ); //	SoundID = 6
	g_engine->m_audio->CreateOrGetSound("Data/Audio/hitmarker_2.mp3"); //						SoundID = 7
}

//-----------------------------------------------------------------------------------------------
void Game::HandleSound(SoundPlaybackID soundID, SoundPriority priority, float soundDuration)
{
	if (soundID == MISSING_SOUND_ID)
		return;

	if (priority == PRIORITY_LOW) // Always play shoot and handle seperately
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

void Game::CreateBlackHole()
{
	float holeRadii[NUM_BLACK_HOLE_SIDES] = {};
	for ( int sideNum = 0; sideNum < NUM_BLACK_HOLE_SIDES; ++sideNum )
	{
		holeRadii[sideNum] = g_rng.RollRandomFloatInRange( 1.9f, 2.0f );
	}

	// Precompute 2D vertex offsets
	constexpr float degreesPerBlackHoleSide = 360.f / ( float )NUM_BLACK_HOLE_SIDES;
	Vec2 blackholeLocalVertPositions[NUM_BLACK_HOLE_SIDES] = {};
	for ( int sideNum = 0; sideNum < NUM_BLACK_HOLE_SIDES; ++sideNum )
	{
		float degrees = degreesPerBlackHoleSide * ( float )sideNum;
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
void Game::GenerateStars()
{
	for ( int starNum = 0; starNum < NEW_MAX_STARS; ++starNum )
	{
		float randX = g_rng.RollRandomFloatInRange( 0.0f, WORLD_SIZE_X);
		float randY = g_rng.RollRandomFloatInRange( 0.0f, WORLD_SIZE_Y);

		int firstVertIndex = ( starNum * 3 ) + 0;
		int secondVertIndex = ( starNum * 3 ) + 1;
		int thirdVertIndex = ( starNum * 3 ) + 2;

		Vertex& vert1 = m_starVerts[firstVertIndex];
		Vertex& vert2 = m_starVerts[secondVertIndex];
		Vertex& vert3 = m_starVerts[thirdVertIndex];

		vert1.m_position = Vec3(randX, randY, 0.f);
		vert2.m_position = Vec3(randX + .1f, randY, 0.f);
		vert3.m_position = Vec3(randX, randY + .1f, 0.f);
	}
}

void Game::RoundTimer(float deltaSeconds)
{
	if(m_playerShip->m_isDead)
		return;

	m_spawnBuffer -= deltaSeconds;
	m_roundTime += deltaSeconds;

	if (m_roundTime >= m_bestRoundTime)
		m_bestRoundTime = m_roundTime;
}
