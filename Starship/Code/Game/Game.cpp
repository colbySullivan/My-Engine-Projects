#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerShip.hpp"
#include <Engine/Core/Engine.hpp>
#include <Engine/Core/ErrorWarningAssert.hpp>
#include "Game/Bullet.hpp"
#include "Game/Asteroid.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/Beetle.hpp"


Game::Game(App* owner)
	: m_app( owner )
{
	m_gameCamera = new Camera;
	g_engine = new Engine;
}

Game::~Game()
{
	delete g_engine;
	g_engine = nullptr;
}

void Game::Startup()
{
	Vec2 worldCenter(WORLD_SIZE_X * 0.5f, WORLD_SIZE_Y * 0.5f);
	m_playerShip = new PlayerShip(this, worldCenter);
	m_beetle = new Beetle(this, Vec2(50.f, 50.f));

	/*for (int i = 0; i < NUM_STARTING_ASTEROIDS; ++i)
	{
		SpawnRandomAsteroid();
	}*/
	UpdateWaves();
}

void Game::Update(float deltaSeconds)
{

	if (m_isAttractMode)
	{
		if (g_engine->m_input->WasKeyJustPressed(' ') || g_engine->m_input->WasKeyJustPressed('N'))
		{
			Startup();
			m_isAttractMode = false;
		}
		else
		{
			CleanupGameEntities();
			RenderAttractMode();
		}
	}

	// Check if we are still in attract mode
	if (m_isAttractMode)
	{
		return;
	}

	if (m_isSlowMo) // T pressed
		deltaSeconds = 1.f / 600.f; // Run at 1/10th the speed

	if (!m_isPaused || m_pauseAfterNextUpdate) // P not pressed or has a run after O is pressed
	{
		m_pauseAfterNextUpdate = false; // Reset run token for simulation step
	}

	KeyboardInput();

	if (m_isSlowMo) // T pressed
		deltaSeconds = 1.f / 600.f; // Run at 1/10th the speed

	if (!m_isPaused || m_pauseAfterNextUpdate)
	{
		UpdateEntities(deltaSeconds);
		CheckBulletsVsAsteroids();
		CheckAsteroidsVsShips();
	}

	if (!m_isPaused || m_pauseAfterNextUpdate) // P not pressed or has a run after O is pressed
	{
		m_pauseAfterNextUpdate = false; // Reset run token for simulation step
	}
	g_engine->m_input->EndFrame();
	m_gameCamera->SetOrthoView(Vec2(0.f,0.f),Vec2(200.f, 100.f));
	DestroyGarbageEntities();
}

void Game::Render() const
{
	g_engine->m_render->BeginCamera(*m_gameCamera);
	Rgba8 backgroundColor = Rgba8(static_cast<unsigned char>(0.f), static_cast<unsigned char>(0.f), static_cast<unsigned char>(0.f), static_cast<unsigned char>(255.f)); // Suppresses error with conversion
	g_engine->m_render->ClearScreen(backgroundColor);
	RenderEntities();
}

void Game::Shutdown()
{

}

bool Game::isAlive(Entity* entity) const
{
	return (entity && !entity->m_isDead);
}

Asteroid* Game::SpawnRandomAsteroids()
{
	for (int astroidIndex = 0; astroidIndex < MAX_ASTEROIDS; ++astroidIndex)
	{
		if (m_asteroid[astroidIndex] == nullptr)
		{
			m_asteroid[astroidIndex] = new Asteroid(this, Vec2(0,0)); //Position will be randomly chosen in astroid class
			return m_asteroid[astroidIndex];
		}
	}
	ERROR_RECOVERABLE("Cannot spawn a new asteroid; all array slots are full");
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

Beetle* Game::SpawnBeetle()
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

void Game::UpdateEntities(float deltaSeconds)
{
	m_playerShip->Update(deltaSeconds);
	m_beetle->Update(deltaSeconds);

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
		if (astroid && !m_playerShip->m_isDead)
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

	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
	{
		Beetle const* beetle = m_beetles[beetleIndex];
		if (beetle)
		{
			beetle->Render();
		}
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
	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
	{
		Beetle const* beetle = m_beetles[beetleIndex];
		if (beetle && beetle->m_isGarbage)
		{
			delete beetle;
			m_beetles[beetleIndex] = nullptr;
		}
	}
}

void Game::UpdateAttractMode(float deltaSeconds)
{

}

void Game::RenderAttractMode() const
{
	Camera attractCamera;
	attractCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(20.f, 10.f));

	g_engine->m_render->BeginCamera( attractCamera );
	DebugDrawLine(Vec2(1.f, 1.f), Vec2(19.f, 9.f), 1.f, Rgba8(255,0,0));

	Vertex fakePlayerShipVerts[NUM_SHIP_VERTS];
	PlayerShip::InitializeLocalPlayerShipsVerts(fakePlayerShipVerts);
	TransformVertexArrayXY3D(NUM_SHIP_VERTS, fakePlayerShipVerts, 0.5f, 30.f, Vec2(10.f, 5.f));
	g_engine->m_render->DrawVertexArray(NUM_SHIP_VERTS, fakePlayerShipVerts);

	g_engine->m_render->EndCamera( attractCamera );
}

void Game::KeyboardInput()
{
	if (m_isAttractMode && (g_engine->m_input->WasKeyJustPressed(KEYCODE_ESC)))
	{
		m_isQuitting = true;
	}

	if (g_engine->m_input->WasKeyJustPressed(KEYCODE_ESC) && !m_isAttractMode)
	{
		m_isAttractMode = true;
		return;
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
		SpawnRandomAsteroids();
	}

	if (g_engine->m_input->WasKeyJustPressed(KEYCODE_F1))
	{
		g_drawDebug = !g_drawDebug;
	}

	if (g_engine->m_input->IsKeyDown(KEYCODE_F8))
	{
		g_drawDebug = !g_drawDebug; // TODO restart game
	}
}

int Game::GetNumLivingEnemies() const
{
	int numLivingEnemies = 0;
	for (int astroidIndex = 0; astroidIndex < MAX_ASTEROIDS; ++astroidIndex)
	{
		Asteroid* astroid = m_asteroid[astroidIndex];
		if (isAlive( astroid ))
		{
			++ numLivingEnemies;
		}
	}
	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
	{
		Beetle* beetle = m_beetles[beetleIndex];
		if (isAlive(beetle))
		{
			++numLivingEnemies;
		}
	}
	return numLivingEnemies;
}

bool Game::IsReadyToStartNextWave() const
{
	int numLivingEnemies = GetNumLivingEnemies();
	return numLivingEnemies == 0;
}

void Game::UpdateWaves()
{
	if (IsReadyToStartNextWave())
	{
		for (int i = 0; i < NUM_STARTING_ASTEROIDS; ++i)
		{
			SpawnRandomAsteroids();
		}
		SpawnBeetle();
			
	}
}
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

	if (m_playerShip)
	{
		Vec2 worldCenter(WORLD_SIZE_X * 0.5f, WORLD_SIZE_Y * 0.5f);
		m_playerShip->m_position = worldCenter;
		m_playerShip->m_isDead = false;
		m_playerShip->m_isGarbage = false;
	}

	DestroyGarbageEntities();
}
