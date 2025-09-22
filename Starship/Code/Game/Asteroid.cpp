#include "Asteroid.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Game/GameCommon.hpp"


Asteroid::Asteroid(Game* owner, Vec2 const& startPos)
	: Entity(owner, startPos)
{
	spawnRandomEdge();
	m_angularVelocity = g_rng.RollRandomFloatInRange(-200,200);
	m_cosmeticRadius = ASTEROID_COSMETIC_RADIUS;
	m_physicsRadius = ASTEROID_PHYSICS_RADIUS;
	m_orientationDegrees = g_rng.RollRandomFloatInRange(0.1f,360.f);
	m_velocity = GetForwardNormal() * ASTEROID_SPEED;
	m_health = 3;

	InitializeLocalVerts();
}

Asteroid::~Asteroid()
{

}

void Asteroid::Update(float deltaSeconds)
{
	m_position += m_velocity * deltaSeconds;
	m_orientationDegrees += (m_angularVelocity * deltaSeconds);
	Entity::Update(deltaSeconds);
	if (IsOffscreen())
	{
		WrapAroundScreen();
	}
}

void Asteroid::Render() const
{
	if (m_isDead)
		return;
	Vertex tempShipWorldVerts[NUM_ASTEROID_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_ASTEROID_VERTS; ++vertIndex)
	{
		tempShipWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}

	TransformVertexArrayXY3D(NUM_ASTEROID_VERTS, tempShipWorldVerts, 1.f, m_orientationDegrees, m_position);
	g_engine->m_render->DrawVertexArray(NUM_ASTEROID_VERTS, tempShipWorldVerts);
	if (m_game->g_drawDebug)
		DebugRender();
}

void Asteroid::InitializeLocalVerts()
{
	// Precompute random radii along each triangle-seam (at each outer vertex)
	float asteroidRadii[NUM_ASTEROID_SIDES] = {};
	for (int sideNum = 0; sideNum < NUM_ASTEROID_SIDES; ++sideNum)
	{
		asteroidRadii[sideNum] = g_rng.RollRandomFloatInRange(m_physicsRadius, m_cosmeticRadius);
	}

	// Precompute 2D vertex offsets
	constexpr float degreesPerAsteroidSide = 360.f / (float)NUM_ASTEROID_SIDES;
	Vec2 asteroidLocalVertPositions[NUM_ASTEROID_SIDES] = {};
	for (int sideNum = 0; sideNum < NUM_ASTEROID_SIDES; ++sideNum)
	{
		float degrees = degreesPerAsteroidSide * (float)sideNum;
		float radius = asteroidRadii[sideNum];
		asteroidLocalVertPositions[sideNum].x = radius * CosDegrees(degrees);
		asteroidLocalVertPositions[sideNum].y = radius * SinDegrees(degrees);
	}

	// Build triangles
	for (int triNum = 0; triNum < NUM_ASTEROID_TRIS; ++triNum)
	{
		int startRadiusIndex = triNum;
		int endRadiusIndex = (triNum + 1) % NUM_ASTEROID_SIDES;
		int firstVertIndex = (triNum * 3) + 0;
		int secondVertIndex = (triNum * 3) + 1;
		int thirdVertIndex = (triNum * 3) + 2;
		Vec2 secondVertOfs = asteroidLocalVertPositions[startRadiusIndex];
		Vec2 thirdVertOfs = asteroidLocalVertPositions[endRadiusIndex];
		m_localVerts[firstVertIndex].m_position = Vec3(0.f, 0.f, 0.f);
		m_localVerts[secondVertIndex].m_position = Vec3(secondVertOfs.x, secondVertOfs.y, 0.f);
		m_localVerts[thirdVertIndex].m_position = Vec3(thirdVertOfs.x, thirdVertOfs.y, 0.f);
	}

	// Set colors
	for (int vertIndex = 0; vertIndex < NUM_ASTEROID_VERTS; ++vertIndex)
	{
		m_localVerts[vertIndex].m_color = Rgba8(100, 100, 100, 255);
	}
}

void Asteroid::WrapAroundScreen()
{
	if (m_position.x > WORLD_SIZE_X + m_cosmeticRadius)
	{
		m_position.x = -m_cosmeticRadius;
	}
	else if (m_position.x < -m_cosmeticRadius)
	{
		m_position.x = WORLD_SIZE_X + m_cosmeticRadius;
	}

	// Wrap vertically
	if (m_position.y > WORLD_SIZE_Y + m_cosmeticRadius)
	{
		m_position.y = -m_cosmeticRadius;
	}
	else if (m_position.y < -m_cosmeticRadius)
	{
		m_position.y = WORLD_SIZE_Y + m_cosmeticRadius;
	}
}

void Asteroid::spawnRandomEdge()
{
	int spawnEdge = g_rng.RollRandomIntInRange(0, 3);

	switch (spawnEdge)
	{
	case 0: // Left
		m_position.x = -m_cosmeticRadius;
		m_position.y = g_rng.RollRandomFloatInRange(0, WORLD_SIZE_Y);
		break;

	case 1: // Right
		m_position.x = WORLD_SIZE_X + m_cosmeticRadius;
		m_position.y = g_rng.RollRandomFloatInRange(0, WORLD_SIZE_Y);
		break;

	case 2: // Top
		m_position.x = g_rng.RollRandomFloatInRange(0, WORLD_SIZE_X);
		m_position.y = WORLD_SIZE_Y + m_cosmeticRadius;
		break;

	case 3: // Bottom
		m_position.x = g_rng.RollRandomFloatInRange(0, WORLD_SIZE_X);
		m_position.y = -m_cosmeticRadius;
		break;
	}
}

