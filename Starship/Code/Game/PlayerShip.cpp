#include "PlayerShip.hpp"
#include <Engine/Core/Vertex.hpp>
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "App.hpp"
#include <Engine/Core/VertexUtils.hpp>
#include "GameCommon.hpp"
#include "Engine/Core/Rgba8.hpp"


PlayerShip::PlayerShip( Game* owner, Vec2 const& startPos)
	: Entity(owner, startPos)
{
	m_physicsRadius = PLAYER_SHIP_PHYSICS_RADIUS;
	m_cosmeticRadius = PLAYER_SHIP_COSMETIC_RADIUS;
	InitializeLocalVerts();
}

PlayerShip::~PlayerShip()
{

}

void PlayerShip::Update(float deltaSeconds)
{
	if (IsOffscreen() || m_health == 0)
	{
		m_isDead = true;
		m_isGarbage = true;
	}

	UpdateFromKeyboard(deltaSeconds);
	BounceOffWalls();
	m_position += m_velocity * deltaSeconds;
}

void PlayerShip::Render() const
{
	if (m_isDead)
		return;

	Vertex tempShipWorldVerts[NUM_SHIP_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; ++vertIndex)
	{
		tempShipWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}

	TransformVertexArrayXY3D(NUM_SHIP_VERTS, tempShipWorldVerts, 1.f, m_orientationDegrees, m_position);
	g_engine->m_render->DrawVertexArray(NUM_SHIP_VERTS, tempShipWorldVerts);
	if(g_theApp->g_drawDebug)
		DebugRender();
}

void PlayerShip::InitializeLocalVerts()
{
	// Nose cone
	m_localVerts[0].m_position = Vec3(1.f, 0.f, 0.f);
	m_localVerts[1].m_position = Vec3(0.f, 1.f, 0.f);
	m_localVerts[2].m_position = Vec3(0.f, -1.f, 0.f);

	// Left wing
	m_localVerts[3].m_position = Vec3(2.f, 1.f, 0.f);
	m_localVerts[4].m_position = Vec3(0.f, 2.f, 0.f);
	m_localVerts[5].m_position = Vec3(-2.f, 1.f, 0.f);

	// Right wing
	m_localVerts[6].m_position = Vec3(2.f, -1.f, 0.f);
	m_localVerts[7].m_position = Vec3(-2.f, -1.f, 0.f);
	m_localVerts[8].m_position = Vec3(0.f, -2.f, 0.f);

	// Body (quad tri 1 of 2)
	m_localVerts[9].m_position = Vec3(0.f, 1.f, 0.f);
	m_localVerts[10].m_position = Vec3(-2.f, -1.f, 0.f);
	m_localVerts[11].m_position = Vec3(0.f, -1.f, 0.f);

	// Body (quad tri 2 of 2)
	m_localVerts[12].m_position = Vec3(0.f, 1.f, 0.f);
	m_localVerts[13].m_position = Vec3(-2.f, 1.f, 0.f);
	m_localVerts[14].m_position = Vec3(-2.f, -1.f, 0.f);

	for (int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; ++vertIndex)
	{
		m_localVerts[vertIndex].m_color = Rgba8(102, 153, 204, 255);
	}
}

void PlayerShip::UpdateFromKeyboard(float deltaSeconds)
{
	if (g_theApp->isKeyJustPressed(' '))
	{
		m_game->SpawnBullet(m_position, m_orientationDegrees);
	}
	if (g_theApp->isKeyDown('S') && !m_isTurningRight)
	{
		m_isTurningLeft = true;
		m_orientationDegrees += PLAYER_SHIP_TURN_SPEED * deltaSeconds;
	}
	if (g_theApp->isKeyDown('F') && !m_isTurningLeft)
	{
		m_isTurningRight = true;
		m_orientationDegrees -= PLAYER_SHIP_TURN_SPEED * deltaSeconds;
	}
	if (g_theApp->isKeyDown('E'))
	{
		Vec2 forwardVec = GetForwardNormal();
		m_velocity += forwardVec * PLAYER_SHIP_ACCELERATION * deltaSeconds;
	}
	else
	{
		m_isTurningLeft = false;
		m_isTurningRight = false;
	}
}

void PlayerShip::BounceOffWalls()
{

}

void PlayerShip::Respawn()
{

}
