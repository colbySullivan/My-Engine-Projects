#include "Beetle.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Game/GameCommon.hpp"


Beetle::Beetle(Game* owner, Vec2 const& startPos)
	: Entity(owner, startPos)
{
	spawnRandomEdge();
	m_angularVelocity = g_rng.RollRandomFloatInRange(-200, 200);
	m_cosmeticRadius = ASTEROID_COSMETIC_RADIUS;
	m_physicsRadius = ASTEROID_PHYSICS_RADIUS;
	m_orientationDegrees = g_rng.RollRandomFloatInRange(0.1f, 360.f);
	m_velocity = GetForwardNormal() * ASTEROID_SPEED;
	m_health = 3;

	InitializeLocalVerts();
}

Beetle::~Beetle()
{

}

void Beetle::Update(float deltaSeconds)
{
	m_position += m_velocity * deltaSeconds;
	m_orientationDegrees += (m_angularVelocity * deltaSeconds);
	if (m_health == 0)
	{
		m_isDead = true;
		m_isGarbage = true;
	}
	if (IsOffscreen())
	{
		WrapAroundScreen();
	}
}

void Beetle::Render() const
{
	if (m_isDead)
		return;
	Vertex tempShipWorldVerts[NUM_BEETLE_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_BEETLE_VERTS; ++vertIndex)
	{
		tempShipWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}

	TransformVertexArrayXY3D(NUM_BEETLE_VERTS, tempShipWorldVerts, 10.f, m_orientationDegrees, m_position);
	g_engine->m_render->DrawVertexArray(NUM_BEETLE_VERTS, tempShipWorldVerts);
	if (m_game->g_drawDebug)
		DebugRender();
}

void Beetle::InitializeLocalVerts()
{
	m_localVerts[0].m_position = Vec3(-1.f, -1.f, 1.f);

	m_localVerts[1].m_position = Vec3(1.f, -1.f, 1.f);

	m_localVerts[2].m_position = Vec3(1.f, 1.f, 1.f);

	m_localVerts[3].m_position = Vec3(-1.f, 1.f, 1.f);
	
	m_localVerts[4].m_position = Vec3(-1.f, -1.f, -1.f);
	
	m_localVerts[5].m_position = Vec3(1.f, -1.f, -1.f);
	
	m_localVerts[6].m_position = Vec3(1.f, 1.f, -1.f);
	
	m_localVerts[7].m_position = Vec3(-1.f, 1.f, -1.f);

	for (int vertIndex = 0; vertIndex < NUM_BEETLE_VERTS; ++vertIndex)
	{
		m_localVerts[vertIndex].m_color = Rgba8(102, 153, 204, 255);
	}
}

void Beetle::WrapAroundScreen()
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

void Beetle::spawnRandomEdge()
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

