#pragma once
#include "App.hpp"
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/Clock.hpp"

class App;
class PlayerShip;
class Asteroid;
class Bullet;
class Entity;
class InputSystem;
class Beetle;
class Debris;
class Wasp;
class RandomNumberGenerator;
class XboxController;
class AudioSystem;
class Interactable;
class BitmapFont;
class Clock;

//-----------------------------------------------------------------------------------------------
enum Game_State
{
	GAMESTATE_ATTRACT,
	GAMESTATE_PLAY,
	GAMESTATE_INVALID,
	NUM_GAMESTATES
};

//------------------------------------------------------------------------------
enum SoundPriority
{
	PRIORITY_LOW,			// Shoot
	PRIORITY_MEDIUM,		// Hit/hurt / Alien death
	PRIORITY_HIGH,			// Respawn / Final death
	PRIORITY_MUSIC,			// Background music game and lobby
	NUM_SOUND_PRIORITIES
};

//-----------------------------------------------------------------------------------------------
class Game
{

public:
	Game();
	~Game();

	void Startup();
	void Update( );
	void Render() const;
	void Shutdown();

	Asteroid* SpawnRandomAsteroids();
	Bullet* SpawnBullet( Vec2 const& pos, float forwardDegrees );
	Beetle* SpawnNewRandomBeetle();
	Wasp* SpawnNewRandomWasp();
	Interactable* SpawnRandomInteractable();
	Debris* SpawnNewDebris(Vec2 pos, Rgba8 color, Vec2 velocity, float size);
	void SpawnDebrisCluster( Vec2 pos, Rgba8 entityColor, Vec2 velocity, int debrisAmount, float size );
	void HandleSound( SoundPlaybackID soundID, SoundPriority priority = PRIORITY_LOW, float soundDuration = 0.2f);

	bool IsAlive( Entity* entity ) const;

	PlayerShip* m_playerShip = nullptr;
	Beetle* m_beetle = nullptr;

	// Game State
	bool				m_isQuitting = false;
	bool				m_isPaused = false;
	bool				m_isSlowMo = false;
	bool				m_pauseAfterNextUpdate = false;
	bool				g_drawDebug = false;
	bool				m_playingEndSound = false;
	bool				m_powerUpScreen = false;

	// Utility
	int					m_roundNumber = 1;
	float				m_alphaTimer = 3;
	float				m_roundEndTimer = 3;
	float				m_camShakeAmount;
	float				m_shipAnimationTimer = 0.0f;
	Game_State			m_currentGameState = GAMESTATE_ATTRACT;
	Game_State			m_nextGameState  = GAMESTATE_ATTRACT;
	float 				m_roundTime;	
	float				m_bestRoundTime = 0.f;
	float				m_spawnBuffer;
	PowerUp				m_currentPowerUp = Num_PowerUps;
	float				m_powerUpTimer = 0.f;
	int					m_enemiesKilled = 0;


	// Audio
	SoundPlaybackID		m_endPlaybackID = MISSING_SOUND_ID;
	SoundPlaybackID		m_lobbyPlaybackID = MISSING_SOUND_ID;
	SoundPlaybackID		m_gameMusicPlaybackID = MISSING_SOUND_ID;
	SoundPlaybackID		m_currentSound = MISSING_SOUND_ID;
	SoundPlaybackID		m_roundChangeSound = MISSING_SOUND_ID;
	SoundPriority		m_currentSoundPriority = PRIORITY_LOW;
	float				m_soundDurationTimer = 0.f;
	SoundPlaybackID		m_shootSound = MISSING_SOUND_ID;
	float				m_shotSoundDurationTimer = 0.f;

	// Textures
	BitmapFont* g_testFont = nullptr;

	// Clock
	Clock* m_gameClock = nullptr;

private:
	void UpdateKeyboardInput( float deltaSeconds, XboxController const& controller );

	void UpdateEntities( float deltaSeconds );
	bool CheckInvincibilityMode();

	void CheckBulletsVsEnemies();
	void CheckBulletsVsEnemies( Bullet& bullet, Entity& enemy );
	void CheckEnemiesVsShips();
	void CheckEnemiesVsShip( Entity& enemy, PlayerShip& ship );
	void CheckInteractablesVsShips();
	void CheckBeetlePush();

	void RenderEntities() const;
	void RenderUI() const;
	void RenderText( const char text[] , Vec2 pos, float height, Rgba8 color ) const;
	void UpdateCameras( float deltaSeconds );

	void UpdateAttractMode( float deltaSeconds );
	void RenderAttractMode( ) const;

	int GetNumLivingEnemies() const;
	bool IsReadyToStartNextWave() const;
	void UpdateWaves();

	void DestroyGarbageEntities();
	void RemoveAllGameEntities();
	void LoadSounds();
	void UpdateBlackHole();
	void GenerateStars();
	void RoundTimer(float deltaSeconds);
	void RenderPauseScreenPowerUp(PowerUp currentPowerUp) const;
	void RenderDeadScreen() const;

	App*			m_app = nullptr;
	Camera*			m_worldCamera = nullptr;
	Camera*			m_screenCamera = nullptr;

	// Entity Arrays
	Bullet*			m_bullets[MAX_BULLETS] = {};
	Asteroid*		m_asteroid[MAX_ASTEROIDS] = {};
	Beetle*			m_beetles[MAX_BEETLES] = {};
	Debris*			m_debris[MAX_DEBRIS] = {};
	Wasp*			m_wasp[MAX_WASP] = {};
	Interactable*	m_interactable[MAX_INTERACTABLES] = {};

	// Black hole
	Vertex			m_blackHoleVerts[NUM_BLACK_HOLE_VERTS];
	Vertex			m_gameBlackHole[NUM_BLACK_HOLE_VERTS];
	int				m_roundBlackHoleAmount = 2;

	// Stars
	Vertex			m_starVerts[NUM_STAR_VERTS];
};