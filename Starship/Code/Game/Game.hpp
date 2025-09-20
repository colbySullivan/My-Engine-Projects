#pragma once
#include "App.hpp"
#include "GameCommon.hpp"

//------------------------------------------------------------------------------

class App;
class PlayerShip;
class Asteroid;
class Bullet;
class Entity;
class InputSystem;


//------------------------------------------------------------------------------

class Game
{
public:
	Game(App* owner);
	~Game();
	void Startup();
	void Update(float deltaSeconds);
	void Render() const;
	void Shutdown();
	bool isAlive(Entity* entity) const;

	
	Asteroid* SpawnRandomAsteroid();
	Bullet* SpawnBullet(Vec2 const& pos, float forwardDegrees);
	PlayerShip*		m_playerShip = nullptr;

	bool			m_isAttractMode = false;
	bool			m_isQuitting = false;
	bool            m_isPaused = false;
	bool            m_isSlowMo = false;
	bool            m_pauseAfterNextUpdate = false;
	bool			g_drawDebug = false;

private:
	void UpdateEntities(float deltaSeconds);
	void CheckBulletsVsAsteroids();
	void CheckBulletsVsAsteroid(Bullet& bullet, Asteroid& asteroid);
	void CheckAsteroidsVsShips();
	void CheckAsteroidVsShip(Asteroid& asteroid, PlayerShip& ship);
	void RenderEntities() const;
	void DestroyGarbageEntities();
	void UpdateAttractMode(float deltaSeconds);
	void RenderAttractMode() const;

	App*			m_app;
	Bullet*			m_bullets[MAX_BULLETS] = {};
	Asteroid*		m_asteroid[MAX_ASTEROIDS] = {};
	

	Camera*			m_gameCamera = nullptr;

	int GetNumLivingEnemies() const;
	bool IsReadyToStartNextWave() const;
	void UpdateWaves();
};