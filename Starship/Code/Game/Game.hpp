#pragma once
#include "App.hpp"
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Audio/AudioSystem.hpp"

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

//------------------------------------------------------------------------------
class Game
{
public:
	Game();
	~Game();

	void Startup();
	void Update( float deltaSeconds );
	void Render() const;
	void Shutdown();

	Asteroid* SpawnRandomAsteroids();
	Bullet* SpawnBullet( Vec2 const& pos, float forwardDegrees );
	Beetle* SpawnNewRandomBeetle();
	Wasp* SpawnNewRandomWasp();
	Debris* SpawnNewDebris( Vec2 pos, Rgba8 color, Vec2 velocity, float size );
	void SpawnDebrisCluster( Vec2 pos, Rgba8 entityColor, Vec2 velocity, int debrisAmount, float size );

	bool isAlive( Entity* entity ) const;

	PlayerShip* m_playerShip = nullptr;
	Beetle* m_beetle = nullptr;
	RandomNumberGenerator g_rng;

	// Game State
	bool				m_isAttractMode = true;
	bool				m_isQuitting = false;
	bool				m_isPaused = false;
	bool				m_isSlowMo = false;
	bool				m_pauseAfterNextUpdate = false;
	bool				g_drawDebug = false;

	// Utility
	int					m_roundNumber = 1;
	float				m_alphaTimer = 3;
	float				m_roundEndTimer = 3;
	float				m_camShakeAmount;


	// Audio
	SoundPlaybackID		m_musicPlaybackID = MISSING_SOUND_ID;


private:
	void KeyboardInput( float deltaSeconds, XboxController const& controller );

	void UpdateEntities( float deltaSeconds );

	void CheckBulletsVsEnemies();
	void CheckBulletsVsEnemies( Bullet& bullet, Entity& enemy );
	void CheckEnemiesVsShips();
	void CheckEnemiesVsShip( Entity& enemy, PlayerShip& ship );
	void CheckBeetlePush();

	void RenderEntities() const;
	void RenderShipLives() const;
	void UpdateCameras( float deltaSeconds );

	bool AttractModeExitEnter( float deltaSeconds,  XboxController const& controller );
	void UpdateAttractMode( float deltaSeconds );
	void RenderAttractMode( float playButtonAlpha ) const;

	int GetNumLivingEnemies() const;
	bool IsReadyToStartNextWave() const;
	void UpdateWaves();

	void DestroyGarbageEntities();
	void CleanupGameEntities();

	App* m_app;
	//Camera* m_gameCamera = nullptr;
	Camera* m_worldCamera = nullptr;
	Camera* m_screenCamera = nullptr;

	// Entity Arrays
	Bullet* m_bullets[MAX_BULLETS] = {};
	Asteroid* m_asteroid[MAX_ASTEROIDS] = {};
	Beetle* m_beetles[MAX_BEETLES] = {};
	Debris* m_debris[MAX_DEBRIS] = {};
	Wasp* m_wasp[MAX_WASP] = {};
};