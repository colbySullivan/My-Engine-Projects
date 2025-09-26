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
	m_entityColor = Rgba8(10, 255, 10, 255);
	InitializeLocalVerts();
	spawnRandomEdge();
}

Wasp::~Wasp()
{

}

void Wasp::Update(float deltaSeconds)
{
	if (!m_game->m_playerShip->m_isDead)
	{
		Vec2 toPlayerPos = m_game->m_playerShip->m_position - m_position;
		Vec2 directionToPlayer = toPlayerPos.GetNormalized();

		m_orientationDegrees = Atan2Degrees(directionToPlayer.y, directionToPlayer.x) - 90.f;

		m_velocity += directionToPlayer * (WASP_ACCELERATION * deltaSeconds);
		m_velocity.x = GetClamped(m_velocity.x, -WASP_MAX_SPEED, WASP_MAX_SPEED);
		m_velocity.y = GetClamped(m_velocity.y, -WASP_MAX_SPEED, WASP_MAX_SPEED);
	}
	m_position += (m_velocity * deltaSeconds);

	if (m_health == 0 && !m_isDead)
	{
		m_isDead = true;
		m_isGarbage = true;
		Die();
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

	TransformVertexArrayXY3D(NUM_WASP_VERTS, tempShipWorldVerts, 1.f, m_orientationDegrees, m_position);
	g_engine->m_render->DrawVertexArray(NUM_WASP_VERTS, tempShipWorldVerts);
	if (m_game->g_drawDebug)
		DebugRender();
}

void Wasp::InitializeLocalVerts()
{
	m_localVerts[0].m_position = Vec3(-2.f, -2.f, 0.f);
	m_localVerts[1].m_position = Vec3(-1.f, 0.f, 0.f);
	m_localVerts[2].m_position = Vec3(0.f, -2.f, 0.f);

	m_localVerts[3].m_position = Vec3(-1.f, -2.f, 0.f);
	m_localVerts[4].m_position = Vec3(0.f, 2.f, 0.f);
	m_localVerts[5].m_position = Vec3(1.f, -2.f, 0.f);

	m_localVerts[6].m_position = Vec3(0.f, -2.f, 0.f);
	m_localVerts[7].m_position = Vec3(1.f, 0.f, 0.f);
	m_localVerts[8].m_position = Vec3(2.f, -2.f, 0.f);

	//m_localVerts[9].m_position = Vec3(0.f, 1.5f, 0.f);
	//m_localVerts[10].m_position = Vec3(0.f, -1.5f, 0.f);
	//m_localVerts[11].m_position = Vec3(1.5f, 0.f, 0.f);


	for (int vertIndex = 0; vertIndex < NUM_WASP_VERTS; ++vertIndex)
	{
		m_localVerts[vertIndex].m_color = Rgba8(m_entityColor.r, m_entityColor.g, m_entityColor.b, m_entityColor.a);
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

