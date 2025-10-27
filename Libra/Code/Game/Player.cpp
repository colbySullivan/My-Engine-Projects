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
	UpdateFromKeyboard(deltaSeconds);
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

	TransformVertexArrayXY3D(NUM_VERTS, tempShipWorldVerts, 1.f, m_orientationDegrees, m_position);
	g_engine->m_render->DrawVertexArray(NUM_VERTS, tempShipWorldVerts);

	if (m_game->g_drawDebug)
	{
		DebugRender();
	}
}

//------------------------------------------------------------------------------
void Player::InitializeLocalVerts()
{

	m_localVerts[0].m_position = Vec3( -0.5f, -0.5f, 0.f );
	m_localVerts[1].m_position = Vec3( 0.5f, -0.5f, 0.f );
	m_localVerts[2].m_position = Vec3( 0.5f, 0.5f, 0.f );

	m_localVerts[3].m_position = Vec3( -0.5f, -0.5f, 0.f );
	m_localVerts[4].m_position = Vec3( -0.5f, 0.5f, 0.f );
	m_localVerts[5].m_position = Vec3( 0.5f, 0.5f, 0.f );
	for ( int vertIndex = 0; vertIndex < 6; ++vertIndex )
	{
		m_localVerts[vertIndex].m_color = Rgba8( 255, 255, 255, 255 );
	}
}

//------------------------------------------------------------------------------
void Player::UpdateFromKeyboard(float deltaSeconds)
{
	Vec2 moveInput(0.f, 0.f);
	bool isAKeyPressed = false;
	if (g_engine->m_input->IsKeyDown('E'))
	{
		moveInput.y += 1.f;
		isAKeyPressed = true;
	}
	if (g_engine->m_input->IsKeyDown('D'))
	{
		moveInput.y -= 1.f;
		isAKeyPressed = true;
	}
	if (g_engine->m_input->IsKeyDown('S'))
	{
		moveInput.x -= 1.f;
		isAKeyPressed = true;
	}
	if (g_engine->m_input->IsKeyDown('F'))
	{
		moveInput.x += 1.f;
		isAKeyPressed = true;

	}
	if (isAKeyPressed)
	{
		UpdateMovement(deltaSeconds, moveInput);
	}
	else
	{
		m_velocity = Vec2(0.f, 0.f);
	}

}

//------------------------------------------------------------------------------
void Player::Respawn()
{

}

//------------------------------------------------------------------------------
void Player::UpdateFromController(float deltaSeconds)
{
	XboxController const& controller = g_engine->m_input->GetController(0);

	float leftStickMagnitude = controller.GetLeftStick().GetMagnitude();
	if (leftStickMagnitude > 0.f)
	{
		Vec2 leftStickPos = controller.GetLeftStick().GetPosition();
		UpdateMovement(deltaSeconds, leftStickPos);
	}
}

//------------------------------------------------------------------------------
void Player::UpdateMovement(float deltaSeconds, Vec2 const& moveInput)
{
	float headingDirection = moveInput.GetOrientationDegrees();

	float angleDelta = GetShortestAngularDispDegrees(m_orientationDegrees, headingDirection);
	float maxTurnThisFrame = PLAYER_TURN_SPEED * deltaSeconds;

	if (fabsf(angleDelta) <= maxTurnThisFrame)
	{
		m_orientationDegrees = headingDirection;
	}
	else
	{
		if (angleDelta > 0.f)
		{
			m_orientationDegrees += maxTurnThisFrame;
		}
		else
		{
			m_orientationDegrees -= maxTurnThisFrame;
		}
	}
	m_velocity = GetForwardNormal() * PLAYER_SPEED;
}
