#include "Beetle.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"


Beetle::Beetle(Game* owner, Vec2 const& startPos)
	: Entity(owner, startPos)
{
	
	m_cosmeticRadius = BEETLE_COSMETIC_RADIUS;
	m_physicsRadius = BEETLE_PHYSICS_RADIUS;
	m_health = 3;
	InitializeLocalVerts();
	spawnRandomEdge();
}

//-----------------------------------------------------------------------------------------------
Beetle::~Beetle()
{

}

//-----------------------------------------------------------------------------------------------
void Beetle::Update(float deltaSeconds)
{
	if (!m_game->m_playerShip->m_isDead)
	{
		Vec2 toPlayerPos = m_game->m_playerShip->m_position - m_position;
		Vec2 directionToPlayer = toPlayerPos.GetNormalized();
		m_orientationDegrees = Atan2Degrees(directionToPlayer.y, directionToPlayer.x) - 90.f;
		m_velocity = directionToPlayer * BEETLE_SPEED;
	}
	m_position += m_velocity * deltaSeconds;

	Entity::Update(deltaSeconds);
}

//-----------------------------------------------------------------------------------------------
void Beetle::Render() const
{
	if (m_isDead)
		return;
	Vertex tempShipWorldVerts[NUM_BEETLE_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_BEETLE_VERTS; ++vertIndex)
	{
		tempShipWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}

	TransformVertexArrayXY3D(NUM_BEETLE_VERTS, tempShipWorldVerts, 3.f, m_orientationDegrees, m_position);
	g_engine->m_render->DrawVertexArray(NUM_BEETLE_VERTS, tempShipWorldVerts);
	if (m_game->g_drawDebug)
		DebugRender();
}

//-----------------------------------------------------------------------------------------------
void Beetle::InitializeLocalVerts()
{
	m_localVerts[0].m_position = Vec3(-0.25f, 0.375f, 0.f);
	m_localVerts[1].m_position = Vec3(-0.5f, -0.375f, 0.f);
	m_localVerts[2].m_position = Vec3(0.25f, 0.375f, 0.f);

	m_localVerts[3].m_position = Vec3(-0.5f, -0.375f, 0.f);
	m_localVerts[4].m_position = Vec3(0.5f, -0.375f, 0.f);
	m_localVerts[5].m_position = Vec3(0.25f, 0.375f, 0.f);


	for (int vertIndex = 0; vertIndex < NUM_BEETLE_VERTS; ++vertIndex)
	{
		m_localVerts[vertIndex].m_color = Rgba8(255, 191, 0, 255);
	}
}

