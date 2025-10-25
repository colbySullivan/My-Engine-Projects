#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"  

//------------------------------------------------------------------------------
Player::Player(Game* owner, Vec2 const& startPos)
	: Entity(owner, startPos)
{
	m_physicsRadius = PLAYER_PHYSICS_RADIUS;
	m_cosmeticRadius = PLAYER_COSMETIC_RADIUS;
	InitializeLocalVerts();
}

//------------------------------------------------------------------------------
Player::~Player()
{

}

//------------------------------------------------------------------------------
void Player::Update([[maybe_unused]] float deltaSeconds)
{
	Entity::Update(deltaSeconds);
	UpdateFromController(deltaSeconds);

	m_position += m_velocity * deltaSeconds;
}

//------------------------------------------------------------------------------
void Player::Render() const
{
	if (m_isDead)
		return;
	Vertex tempShipWorldVerts[NUM_VERTS];

	for (int vertIndex = 0; vertIndex < NUM_VERTS; ++vertIndex)
	{
		tempShipWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}

	TransformVertexArrayXY3D(NUM_VERTS, tempShipWorldVerts, 3.f, m_orientationDegrees, m_position);
	g_engine->m_render->DrawVertexArray(NUM_VERTS, tempShipWorldVerts);

	if (m_game->g_drawDebug)
	{
		DebugRender();
	}
}

//------------------------------------------------------------------------------
void Player::InitializeLocalVerts()
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

	for (int vertIndex = 0; vertIndex < NUM_VERTS; ++vertIndex)
	{
		m_localVerts[vertIndex].m_color = Rgba8(102, 153, 204, 255);
	}
}

//------------------------------------------------------------------------------
void Player::UpdateFromKeyboard(float deltaSeconds)
{
	if (g_engine->m_input->IsKeyDown('S'))
	{
		m_orientationDegrees += PLAYER_TURN_SPEED * deltaSeconds;
	}

	if (g_engine->m_input->IsKeyDown('F'))
	{
		m_orientationDegrees -= PLAYER_TURN_SPEED * deltaSeconds;
	}

	if (g_engine->m_input->IsKeyDown('E'))
	{
		Vec2 forwardVec = GetForwardNormal();
		m_velocity += forwardVec * PLAYER_ACCELERATION * deltaSeconds;
	}
}

//------------------------------------------------------------------------------
void Player::Respawn()
{

}

//------------------------------------------------------------------------------
void Player::UpdateFromController(float deltaSeconds)
{
	XboxController const& controller = g_engine->m_input->GetController(0); // #ToDo: support multiple players?
	g_engine->m_input->GetController(0);

	// Respawn
	if (m_isDead)
	{
		if (controller.WasButtonJustPressed(XboxButtonID::A) && m_lives > 0)
		{
			Respawn();
		}
		return;
	}

	// Drive
	float leftStickMagnitude = controller.GetLeftStick().GetMagnitude();
	if (leftStickMagnitude > 0.f)
	{
		m_thrustFraction = leftStickMagnitude;
		m_orientationDegrees = controller.GetLeftStick().GetOrientationDegrees();

		Vec2 forwardVec = GetForwardNormal();
		m_velocity += forwardVec * PLAYER_ACCELERATION * m_thrustFraction * deltaSeconds;
	}
}