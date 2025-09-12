#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerShip.hpp"
#include <Engine/Core/Engine.hpp>
#include <Engine/Core/ErrorWarningAssert.hpp>
#include "Game/Bullet.hpp"
#include "Game/Asteroid.hpp"

Game::Game(App* owner)
	: m_app( owner )
{

}

Game::~Game()
{

}

void Game::Startup()
{
	Vec2 worldCenter(WORLD_SIZE_X * 0.5f, WORLD_SIZE_Y * 0.5f);
	m_playerShip = new PlayerShip(this, worldCenter);

	for (int i = 0; i < NUM_STARTING_ASTEROIDS; ++i)
	{
		SpawnRandomAsteroid();
	}
}

void Game::Update(float deltaSeconds)
{
	//if (m_isSlowMo) // T pressed
	//	deltaSeconds = 1.f / 600.f; // Run at 1/10th the speed
	//if (!m_isPaused || m_pauseAfterNextUpdate) // P not pressed or has a run after O is pressed
	//{
	//	m_pauseAfterNextUpdate = false; // Reset run token for simulation step
	//}
	UpdateEntities(deltaSeconds);
	// m_playerShip
	// g_drawDebug = !g_drawDebug;
	DestroyGarbageEntities();
}

void Game::Render() const
{
	RenderEntities();	
}

void Game::Shutdown()
{

}

Asteroid* Game::SpawnRandomAsteroid()
{
	for (int astroidIndex = 0; astroidIndex < MAX_ASTEROIDS; ++astroidIndex)
	{
		if (m_asteroid[astroidIndex] == nullptr)
		{
			m_asteroid[astroidIndex] = new Asteroid(this, Vec2(0,0)); //Position will be randomly chosen in astroid class
			return m_asteroid[astroidIndex];
		}
	}
	ERROR_RECOVERABLE("Cannot spawn a new bullet; all array slots are full");
	return nullptr;
}

Bullet* Game::SpawnBullet(Vec2 const& pos, float forwardDegrees)
{
	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
        {
           Bullet* &bullet = m_bullets[bulletIndex];
            if (!bullet)
            {
                bullet = new Bullet(this, pos);
                bullet->m_orientationDegrees = forwardDegrees;
                bullet->m_velocity.x = BULLET_SPEED * CosDegrees(forwardDegrees);
                bullet->m_velocity.y = BULLET_SPEED * SinDegrees(forwardDegrees);
                return bullet;
            }
        }

        ERROR_AND_DIE("Cannot spawn a new bullet; all slots are full");
}

void Game::UpdateEntities(float deltaSeconds)
{
	m_playerShip->Update(deltaSeconds);
	for (int astroidIndex = 0; astroidIndex < MAX_ASTEROIDS; ++astroidIndex)
	{
		Asteroid* astroid = m_asteroid[astroidIndex];
		if (astroid)
		{
			astroid->Update(deltaSeconds);
		}
	}
}

void Game::RenderEntities() const
{
	/*for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
	{
		Bullet const* bullet = m_bullets[bulletIndex];
		if (bullet)
		{
			bullet->Render();
		}
	}*/
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
		
}

void Game::DestroyGarbageEntities()
{
	
}

