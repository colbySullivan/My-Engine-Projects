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
	UpdateEntities(deltaSeconds);
	CheckBulletsVsAsteroids();
	CheckAsteroidsVsShips();
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

 	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
	{
		Bullet* bullet = m_bullets[bulletIndex];
		if (bullet)
		{
			bullet->Update(deltaSeconds);
		}
		
	}
}

void Game::CheckBulletsVsAsteroids()
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
					CheckBulletsVsAsteroid(*bullet, *astroid);
				}

			}

		}
	}
}

void Game::CheckBulletsVsAsteroid(Bullet& bullet, Asteroid& asteroid)
{
	if (DoDiscsOverlap(asteroid.m_position, asteroid.m_physicsRadius, bullet.m_position, bullet.m_physicsRadius))
	{
		bullet.m_isDead = true;
		bullet.m_isGarbage = true;
		asteroid.m_health -= 1;
	}
}

void Game::CheckAsteroidsVsShips()
{
	for (int astroidIndex = 0; astroidIndex < MAX_ASTEROIDS; ++astroidIndex)
	{
		Asteroid* astroid = m_asteroid[astroidIndex];
		if (astroid)
		{
			CheckAsteroidVsShip(*astroid, *m_playerShip);
		}
	}
}

void Game::CheckAsteroidVsShip(Asteroid& asteroid, PlayerShip& ship)
{
	if (DoDiscsOverlap(asteroid.m_position, asteroid.m_physicsRadius, ship.m_position, ship.m_physicsRadius))
	{
		ship.m_health -= 1;
		asteroid.m_health -= 1;
	}
}

void Game::RenderEntities() const
{
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
		
}

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
}

