#include "Wasp.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"


Wasp::Wasp(Game* owner, Vec2 const& startPos)
	: Entity(owner, startPos)
{

	m_cosmeticRadius = WASP_COSMETIC_RADIUS;
	m_physicsRadius = WASP_PHYSICS_RADIUS;
	m_health = 3;
	InitializeLocalVerts();
	spawnRandomEdge();
}

Wasp::~Wasp()
{

}

void Wasp::Update(float deltaSeconds)
{
	Vec2 toPlayerPos = m_game->m_playerShip->m_position - m_position;
	Vec2 directionToPlayer = toPlayerPos.GetNormalized();

	m_orientationDegrees = Atan2Degrees(directionToPlayer.y, directionToPlayer.x) - 90.f; //TODO offset of trapezoid
	m_velocity = directionToPlayer * WASP_SPEED;
	m_position += m_velocity * deltaSeconds;

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

void Wasp::Render() const
{
	if (m_isDead)
		return;
	Vertex tempShipWorldVerts[NUM_WASP_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_WASP_VERTS; ++vertIndex)
	{
		tempShipWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}

	TransformVertexArrayXY3D(NUM_WASP_VERTS, tempShipWorldVerts, 3.f, m_orientationDegrees, m_position);
	g_engine->m_render->DrawVertexArray(NUM_WASP_VERTS, tempShipWorldVerts);
	if (m_game->g_drawDebug)
		DebugRender();
}

void Wasp::InitializeLocalVerts()
{
	m_localVerts[0].m_position = Vec3(-0.25f, 0.375f, 0.f);
	m_localVerts[1].m_position = Vec3(-0.5f, -0.375f, 0.f);
	m_localVerts[2].m_position = Vec3(0.25f, 0.375f, 0.f);

	m_localVerts[3].m_position = Vec3(-0.5f, -0.375f, 0.f);
	m_localVerts[4].m_position = Vec3(0.5f, -0.375f, 0.f);
	m_localVerts[5].m_position = Vec3(0.25f, 0.375f, 0.f);


	for (int vertIndex = 0; vertIndex < NUM_WASP_VERTS; ++vertIndex)
	{
		m_localVerts[vertIndex].m_color = Rgba8(255, 153, 204, 255);
	}
}

void Wasp::WrapAroundScreen()
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

void Wasp::spawnRandomEdge()
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

