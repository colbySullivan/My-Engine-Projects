#pragma once
#include "App.hpp"
#include "GameCommon.hpp"

//------------------------------------------------------------------------------

class App;
class PlayerShip;
class Asteroid;
class Bullet;
class Entity;


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

	
	Asteroid* SpawnRandomAsteroid();
	Bullet* SpawnBullet(Vec2 const& pos, float forwardDegrees);
	PlayerShip*		m_playerShip = nullptr;

private:
	void UpdateEntities(float deltaSeconds);
	void CheckBulletsVsAsteroids();
	void CheckBulletsVsAsteroid(Bullet& bullet, Asteroid& asteroid);
	void CheckAsteroidsVsShips();
	void CheckAsteroidVsShip(Asteroid& asteroid, PlayerShip& ship);
	void RenderEntities() const;
	void DestroyGarbageEntities();

	App*			m_app;
	Bullet*			m_bullets[MAX_BULLETS] = {};
	Asteroid*		m_asteroid[MAX_ASTEROIDS] = {};
	

	Camera*			m_gameCamera = nullptr;
};