#include "PlayerShip.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "App.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Input/InputSystem.hpp"


PlayerShip::PlayerShip( Game* owner, Vec2 const& startPos)
	: Entity(owner, startPos)
{
	m_physicsRadius = PLAYER_SHIP_PHYSICS_RADIUS;
	m_cosmeticRadius = PLAYER_SHIP_COSMETIC_RADIUS;
	m_entityColor = Rgba8(102, 153, 204, 255);
	m_debrisAmount = 12;
	InitializeLocalPlayerShipsVerts(m_localVerts);
}

PlayerShip::~PlayerShip()
{

}

void PlayerShip::Update(float deltaSeconds)
{
	Entity::Update(deltaSeconds);
	if (g_engine->m_input->WasKeyJustPressed('N')) // F8
	{
		Respawn();
	}
	if (!m_isDead)
		UpdateFromKeyboard(deltaSeconds);
	BounceOffWalls();
	m_position += m_velocity * deltaSeconds;
}

void PlayerShip::Render() const
{
	Vertex tempShipWorldVerts[NUM_SHIP_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; ++vertIndex)
	{
		tempShipWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}

	TransformVertexArrayXY3D(NUM_SHIP_VERTS, tempShipWorldVerts, 1.f, m_orientationDegrees, m_position);
	if (!m_isDead)
		g_engine->m_render->DrawVertexArray(NUM_SHIP_VERTS, tempShipWorldVerts);
	if(m_game->g_drawDebug)
		DebugRender();
}

void PlayerShip::InitializeLocalPlayerShipsVerts(Vertex* vertArray)
{
	// Nose cone
	vertArray[0].m_position = Vec3(1.f, 0.f, 0.f);
	vertArray[1].m_position = Vec3(0.f, 1.f, 0.f);
	vertArray[2].m_position = Vec3(0.f, -1.f, 0.f);

	// Left wing
	vertArray[3].m_position = Vec3(2.f, 1.f, 0.f);
	vertArray[4].m_position = Vec3(0.f, 2.f, 0.f);
	vertArray[5].m_position = Vec3(-2.f, 1.f, 0.f);

	// Right wing
	vertArray[6].m_position = Vec3(2.f, -1.f, 0.f);
	vertArray[7].m_position = Vec3(-2.f, -1.f, 0.f);
	vertArray[8].m_position = Vec3(0.f, -2.f, 0.f);

	// Body (quad tri 1 of 2)
	vertArray[9].m_position = Vec3(0.f, 1.f, 0.f);
	vertArray[10].m_position = Vec3(-2.f, -1.f, 0.f);
	vertArray[11].m_position = Vec3(0.f, -1.f, 0.f);

	// Body (quad tri 2 of 2)
	vertArray[12].m_position = Vec3(0.f, 1.f, 0.f);
	vertArray[13].m_position = Vec3(-2.f, 1.f, 0.f);
	vertArray[14].m_position = Vec3(-2.f, -1.f, 0.f);

	for (int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; ++vertIndex)
	{
		vertArray[vertIndex].m_color = Rgba8(102, 153, 204, 255);
	}
}

void PlayerShip::UpdateFromKeyboard(float deltaSeconds)
{
	if (g_engine->m_input->WasKeyJustPressed(' '))
	{
		m_game->SpawnBullet(m_position, m_orientationDegrees);
	}
	if (g_engine->m_input->IsKeyDown('S'))
	{
		m_orientationDegrees += PLAYER_SHIP_TURN_SPEED * deltaSeconds;
	}
	if (g_engine->m_input->IsKeyDown('F'))
	{
		m_orientationDegrees -= PLAYER_SHIP_TURN_SPEED * deltaSeconds;
	}
	if (g_engine->m_input->IsKeyDown('E'))
	{
		Vec2 forwardVec = GetForwardNormal();
		m_velocity += forwardVec * PLAYER_SHIP_ACCELERATION * deltaSeconds;
	}
}

void PlayerShip::BounceOffWalls()
{
	if (m_position.x >= WORLD_SIZE_X - m_cosmeticRadius || m_position.x < 0 + m_cosmeticRadius)
	{
		m_velocity.x = -m_velocity.x;
	}
	if (m_position.y >= WORLD_SIZE_Y - m_cosmeticRadius || m_position.y < 0 + m_cosmeticRadius)
	{
		m_velocity.y = -m_velocity.y;
	}
}

void PlayerShip::Respawn()
{
	if (m_isDead)
	{
		m_position = Vec2(WORLD_SIZE_X * 0.5f, WORLD_SIZE_Y * 0.5f);
		m_velocity = Vec2(0, 0);
		m_orientationDegrees = 0.f;
		m_isDead = false;
		m_health = 1;
		m_lives -= 1;
	}
}
