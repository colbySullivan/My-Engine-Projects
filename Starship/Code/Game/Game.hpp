#pragma once
#include "App.hpp"

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

private:
	void UpdateEntities(float deltaSeconds);
	void CheckBulletsVsAsteroids();
	void CheckBulletsVsAsteroid(Bullet& bullet, Asteroid& asteroid);
	void CheckAsteroidsVsShips();
	void CheckAsteroidVsShip(Asteroid& asteroid, PlayerShip& ship);
	void RenderEntities() const;

	App* m_app;
	PlayerShip* m_playerShip = nullptr; // Just one player ship (for now...)

};