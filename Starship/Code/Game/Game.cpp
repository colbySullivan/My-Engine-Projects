#include "Game.hpp"
#include "GameCommon.hpp"
#include "PlayerShip.hpp"

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

	/*for (int i = 0; i < NUM_STARTING_ASTEROIDS; ++i)
	{
		SpawnRandomAsteroid();
	}*/
}

void Game::Update(float deltaSeconds)
{

}

void Game::Render() const
{
	RenderEntities();
}

void Game::Shutdown()
{

}

//Asteroid* Game::SpawnRandomAsteroid()
//{
//
//}

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

	if (m_playerShip)
		m_playerShip->Render();
}

