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
	InitializePlayerVerts();
	InitializeTurretVerts();
	m_playerTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/Test_StbiFlippedAndOpenGL.png" );
}

//------------------------------------------------------------------------------
Player::~Player()
{

}

//------------------------------------------------------------------------------
void Player::Update([[maybe_unused]] float deltaSeconds)
{
	m_isMoving = false;
	m_velocity = GetForwardNormal();
	Entity::Update(deltaSeconds);
	UpdateFromKeyboard(deltaSeconds);
	UpdateFromController(deltaSeconds);

	if ( !m_isMoving )
	{
		m_velocity = Vec2( 0.f, 0.f );
	}

	m_position += m_velocity * deltaSeconds;
}

//------------------------------------------------------------------------------
void Player::Render() const
{
	RenderPlayer();
	//RenderTurret();
}

//-----------------------------------------------------------------------------------------------
void Player::RenderPlayer() const
{
	//g_engine->m_render->DrawVertexArray( testTextureVerts );

	if ( m_isDead )
		return;
	Vertex tempShipWorldVerts[NUM_PLAYER_VERTS];
	//std::vector<Vertex> tempShipWorldVerts;

	for ( int vertIndex = 0; vertIndex < NUM_PLAYER_VERTS; ++vertIndex )
	{
		tempShipWorldVerts[vertIndex] = m_playerVerts[vertIndex];
		//tempShipWorldVerts.push_back(m_playerVerts[vertIndex]);
	}

	/*Texture* testTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/Test_StbiFlippedAndOpenGL.png" );*/
	//std::vector<Vertex> testTextureVerts;
	//AABB2 texturedAABB2( 0.f, 0.f, 1.f, 1.f );
	TransformVertexArrayXY3D( NUM_PLAYER_VERTS, tempShipWorldVerts, 1.f, m_orientationDegrees, m_position );
	//AABB2 texturedAABB2( m_position.x - 0.5f, m_position.y - 0.5f, m_position.x + 0.5f, m_position.y + 0.5f );
	g_engine->m_render->BindTexture( m_playerTexture );
	g_engine->m_render->DrawVertexArray( NUM_PLAYER_VERTS, tempShipWorldVerts );

	g_engine->m_render->BindTexture( nullptr );

	if ( m_game->g_drawDebug )
	{
		DebugRender();
	}

}

//-----------------------------------------------------------------------------------------------
void Player::RenderTurret() const
{
	if ( m_isDead )
		return;
	Vertex tempTurretWorldVerts[NUM_TURRET_VERTS];

	for ( int vertIndex = 0; vertIndex < NUM_TURRET_VERTS; ++vertIndex )
	{
		tempTurretWorldVerts[vertIndex] = m_turretVerts[vertIndex];
	}

	TransformVertexArrayXY3D( NUM_TURRET_VERTS, tempTurretWorldVerts, 0.8f, m_turretOrientationDegrees, m_position );
	g_engine->m_render->DrawVertexArray( NUM_TURRET_VERTS, tempTurretWorldVerts );

	if ( m_game->g_drawDebug )
	{
		DebugRender();
	}
}

bool Player::IsPlayer()
{
	return true;
}

//------------------------------------------------------------------------------
void Player::InitializePlayerVerts()
{

	m_playerVerts[0].m_position = Vec3( -0.5f, -0.5f, 0.f );
	m_playerVerts[1].m_position = Vec3( 0.5f, -0.5f, 0.f );
	m_playerVerts[2].m_position = Vec3( 0.5f, 0.5f, 0.f );

	m_playerVerts[3].m_position = Vec3( -0.5f, -0.5f, 0.f );
	m_playerVerts[4].m_position = Vec3( -0.5f, 0.5f, 0.f );
	m_playerVerts[5].m_position = Vec3( 0.5f, 0.5f, 0.f );
	for ( int vertIndex = 0; vertIndex < NUM_PLAYER_VERTS; ++vertIndex )
	{
		m_playerVerts[vertIndex].m_color = Rgba8( 255, 255, 255, 255 );
	}
}

//-----------------------------------------------------------------------------------------------
void Player::InitializeTurretVerts()
{
	m_turretVerts[0].m_position = Vec3( -0.5f, -0.5f, 0.f );
	m_turretVerts[1].m_position = Vec3( 0.5f, -0.5f, 0.f );
	m_turretVerts[2].m_position = Vec3( 0.5f, 0.5f, 0.f );

	m_turretVerts[3].m_position = Vec3( -0.5f, -0.5f, 0.f );
	m_turretVerts[4].m_position = Vec3( -0.5f, 0.5f, 0.f );
	m_turretVerts[5].m_position = Vec3( 0.5f, 0.5f, 0.f );

	for ( int vertIndex = 0; vertIndex < NUM_TURRET_VERTS; ++vertIndex )
	{
		m_turretVerts[vertIndex].m_color = Rgba8( 255, 255, 255, 255 );
	}
}

//------------------------------------------------------------------------------
void Player::UpdateFromKeyboard( [[maybe_unused]] float deltaSeconds )
{
	// Player controller
	if (g_engine->m_input->IsKeyDown('E'))
	{
		m_orientationDegrees = GetTurnedTowardDegrees(m_orientationDegrees, 90.f, 1.f);
		m_isMoving = true;
	}
	if (g_engine->m_input->IsKeyDown('D'))
	{
		m_orientationDegrees = GetTurnedTowardDegrees(m_orientationDegrees, -90.f, 1.f);
		m_isMoving = true;
	}
	if (g_engine->m_input->IsKeyDown('S'))
	{
		m_orientationDegrees = GetTurnedTowardDegrees(m_orientationDegrees, -180.f, 1.f);
		m_isMoving = true;
	}
	if (g_engine->m_input->IsKeyDown('F'))
	{
		m_orientationDegrees = GetTurnedTowardDegrees(m_orientationDegrees, 0.f, 1.f);
		m_isMoving = true;
	}

	// Turret controller
	if ( g_engine->m_input->IsKeyDown( 'I' ) )
	{
		m_turretOrientationDegrees = GetTurnedTowardDegrees( m_turretOrientationDegrees, 90.f, 1.f );
	}
	if ( g_engine->m_input->IsKeyDown( 'K' ) )
	{
		m_turretOrientationDegrees = GetTurnedTowardDegrees( m_turretOrientationDegrees, -90.f, 1.f );
	}
	if ( g_engine->m_input->IsKeyDown( 'J' ) )
	{
		m_turretOrientationDegrees = GetTurnedTowardDegrees( m_turretOrientationDegrees, -180.f, 1.f );
	}
	if ( g_engine->m_input->IsKeyDown( 'L' ) )
	{
		m_turretOrientationDegrees = GetTurnedTowardDegrees( m_turretOrientationDegrees, 0.f, 1.f );
	}
}

//------------------------------------------------------------------------------
void Player::Respawn()
{

}

//------------------------------------------------------------------------------
void Player::UpdateFromController([[maybe_unused]] float deltaSeconds)
{
	XboxController const& controller = g_engine->m_input->GetController(0);

	float leftStickMagnitude = controller.GetLeftStick().GetMagnitude();
	if (leftStickMagnitude > 0.f)
	{
		m_isMoving = true;
		Vec2 leftStickPos = controller.GetLeftStick().GetPosition();
		m_orientationDegrees = GetTurnedTowardDegrees( m_orientationDegrees, leftStickPos.GetOrientationDegrees(), 1.f );
	}

	float rightStickMagnitude = controller.GetRightStick().GetMagnitude();
	if ( rightStickMagnitude > 0.f )
	{
		Vec2 rightStickPos = controller.GetRightStick().GetPosition();
		m_turretOrientationDegrees = GetTurnedTowardDegrees( m_turretOrientationDegrees, rightStickPos.GetOrientationDegrees(), 1.f );
	}
}