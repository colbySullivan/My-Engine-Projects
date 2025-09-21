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
class Beetle;


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

	
	Asteroid* SpawnRandomAsteroids();
	Bullet* SpawnBullet(Vec2 const& pos, float forwardDegrees);
	Beetle* SpawnBeetle();
	PlayerShip*		m_playerShip = nullptr;
	Beetle*			m_beetle = nullptr;

	bool			m_isAttractMode = true;
	bool			m_isQuitting = false;
	bool            m_isPaused = false;
	bool            m_isSlowMo = false;
	bool            m_pauseAfterNextUpdate = false;
	bool			g_drawDebug = false;

private:
	void UpdateEntities(float deltaSeconds);
	void CheckBulletsVsEnemies();
	void CheckBulletsVsEnemies(Bullet& bullet, Entity& enemy);
	void CheckEnemiesVsShips();
	void CheckEnemiesVsShip(Entity& enemy, PlayerShip& ship);
	void RenderEntities() const;
	void DestroyGarbageEntities();
	void UpdateAttractMode(float deltaSeconds);
	void RenderAttractMode() const;
	void KeyboardInput();

	App*			m_app;
	Bullet*			m_bullets[MAX_BULLETS] = {};
	Asteroid*		m_asteroid[MAX_ASTEROIDS] = {};
	Beetle*			m_beetles[MAX_BEETLES] = {};
	

	Camera*			m_gameCamera = nullptr;

	int GetNumLivingEnemies() const;
	bool IsReadyToStartNextWave() const;
	void UpdateWaves();
	void CleanupGameEntities();
};