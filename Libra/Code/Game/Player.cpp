#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"  

//------------------------------------------------------------------------------
Player::Player(Game* owner, Vec2 const& startPos, float orientationDegrees, EntityFaction faction, Map* map, EntityType type )
	: Entity(owner, startPos, orientationDegrees, faction, map, type)
{
	m_physicsRadius = PLAYER_PHYSICS_RADIUS;
	m_cosmeticRadius = PLAYER_COSMETIC_RADIUS;
	m_isPushedByWalls = true;
	m_isPushedByEntities = true;
	m_doesPushEntities = true;
	m_isHitByBullets = true;
	m_health = 100;
	m_bulletCooldown = 0.1f;
	InitializePlayerVerts();
	InitializeTurretVerts();
	m_faction = faction;
	m_bodyTexture = m_game->m_playerBodyTexture;
	m_turretTexture = m_game->m_playerTurretTexture;

	g_engine->m_render->BindTexture( nullptr );
}

//------------------------------------------------------------------------------
Player::~Player()
{

}

//------------------------------------------------------------------------------
void Player::Update([[maybe_unused]] float deltaSeconds)
{
	m_isMoving = false;
	m_velocity = GetForwardNormal() * PLAYER_SPEED;
	Entity::Update(deltaSeconds);
	UpdateFromKeyboard(deltaSeconds);
	UpdateFromController(deltaSeconds);

	if ( !m_isMoving )
	{
		m_velocity = Vec2( 0.f, 0.f );
	}
	if ( g_engine->m_input->IsKeyDown( ' ' ) || g_engine->m_input->GetController( 0 ).GetRightTrigger() > 0.5f )
	{
 		TryShoot( m_turretOrientationDegrees, deltaSeconds, m_faction );
	}
	m_position += m_velocity * deltaSeconds;
}

//------------------------------------------------------------------------------
void Player::Render() const
{
	RenderPlayer();
	RenderTurret();
}

//-----------------------------------------------------------------------------------------------
void Player::RenderPlayer() const
{
	if ( m_isDead )
		return;

	Vertex tempShipWorldVerts[NUM_PLAYER_VERTS];

	for ( int vertIndex = 0; vertIndex < NUM_PLAYER_VERTS; ++vertIndex )
	{
		tempShipWorldVerts[vertIndex] = m_playerVerts[vertIndex];
	}
	TransformVertexArrayXY3D( NUM_PLAYER_VERTS, tempShipWorldVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_render->BindTexture( m_bodyTexture );
	g_engine->m_render->DrawVertexArray(NUM_PLAYER_VERTS, tempShipWorldVerts );
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

	TransformVertexArrayXY3D( NUM_PLAYER_VERTS, tempTurretWorldVerts, 1.f, m_turretOrientationDegrees, m_position );
	g_engine->m_render->BindTexture( m_turretTexture );
	g_engine->m_render->DrawVertexArray( NUM_TURRET_VERTS, tempTurretWorldVerts );
	g_engine->m_render->BindTexture( nullptr );

	if ( m_game->g_drawDebug )
	{
		DebugRender();
	}
}

//-----------------------------------------------------------------------------------------------
bool Player::PlayerControlKeyboard()
{
	if ( g_engine->m_input->IsKeyDown( 'E' ) && g_engine->m_input->IsKeyDown( 'S' ) )
	{
		m_orientationDegrees = GetTurnedTowardDegrees( m_orientationDegrees, 135.f, 1.f );
		m_desiredMoveDirection = Vec2( -1.f, 1.f );
		return true;
	}
	if ( g_engine->m_input->IsKeyDown( 'E' ) && g_engine->m_input->IsKeyDown( 'F' ) )
	{
		m_orientationDegrees = GetTurnedTowardDegrees( m_orientationDegrees, 45.f, 1.f );
		m_desiredMoveDirection = Vec2( 1.f, 1.f );
		return true;
	}
	if ( g_engine->m_input->IsKeyDown( 'F' ) && g_engine->m_input->IsKeyDown( 'D' ) )
	{
		m_orientationDegrees = GetTurnedTowardDegrees( m_orientationDegrees, -45.f, 1.f );
		m_desiredMoveDirection = Vec2( 1.f, -1.f );
		return true;
	}
	if ( g_engine->m_input->IsKeyDown( 'D' ) && g_engine->m_input->IsKeyDown( 'S' ) )
	{
		m_orientationDegrees = GetTurnedTowardDegrees( m_orientationDegrees, -135.f, 1.f );
		m_desiredMoveDirection = Vec2( -1.f, -1.f );
		return true;
	}

	if ( g_engine->m_input->IsKeyDown( 'E' ) )
	{
		m_orientationDegrees = GetTurnedTowardDegrees( m_orientationDegrees, 90.f, 1.f );
		m_desiredMoveDirection = Vec2( 0.f, 1.f );
		return true;
	}
	if ( g_engine->m_input->IsKeyDown( 'D' ) )
	{
		m_orientationDegrees = GetTurnedTowardDegrees( m_orientationDegrees, -90.f, 1.f );
		m_desiredMoveDirection = Vec2( 0.f, -1.f );
		return true;
	}
	if ( g_engine->m_input->IsKeyDown( 'S' ) )
	{
		m_orientationDegrees = GetTurnedTowardDegrees( m_orientationDegrees, -180.f, 1.f );
		m_desiredMoveDirection = Vec2( -1.f, 0.f );
		return true;
	}
	if ( g_engine->m_input->IsKeyDown( 'F' ) )
	{
		m_orientationDegrees = GetTurnedTowardDegrees( m_orientationDegrees, 0.f, 1.f );
		m_desiredMoveDirection = Vec2( 1.f, 0.f );
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------------------------
bool Player::TurretControlKeyboard()
{
	if ( g_engine->m_input->IsKeyDown( 'I' ) && g_engine->m_input->IsKeyDown( 'J' ) )
	{
		m_turretOrientationDegrees = GetTurnedTowardDegrees( m_turretOrientationDegrees, 135.f, 1.f );
		m_desiredTurretDirection = Vec2( -1.f, 1.f );
		return true;
	}
	if ( g_engine->m_input->IsKeyDown( 'I' ) && g_engine->m_input->IsKeyDown( 'L' ) )
	{
		m_turretOrientationDegrees = GetTurnedTowardDegrees( m_turretOrientationDegrees, 45.f, 1.f );
		m_desiredTurretDirection = Vec2( 1.f, 1.f );
		return true;
	}
	if ( g_engine->m_input->IsKeyDown( 'K' ) && g_engine->m_input->IsKeyDown( 'L' ) )
	{
		m_turretOrientationDegrees = GetTurnedTowardDegrees( m_turretOrientationDegrees, -45.f, 1.f );
		m_desiredTurretDirection = Vec2( 1.f, -1.f );
		return true;
	}
	if ( g_engine->m_input->IsKeyDown( 'K' ) && g_engine->m_input->IsKeyDown( 'J' ) )
	{
		m_turretOrientationDegrees = GetTurnedTowardDegrees( m_turretOrientationDegrees, -135.f, 1.f );
		m_desiredTurretDirection = Vec2( -1.f, -1.f );
		return true;
	}

	if ( g_engine->m_input->IsKeyDown( 'I' ) )
	{
		m_turretOrientationDegrees = GetTurnedTowardDegrees( m_turretOrientationDegrees, 90.f, 1.f );
		m_desiredTurretDirection = Vec2( 0.f, 1.f );
		return true;
	}
	if ( g_engine->m_input->IsKeyDown( 'K' ) )
	{
		m_turretOrientationDegrees = GetTurnedTowardDegrees( m_turretOrientationDegrees, -90.f, 1.f );
		m_desiredTurretDirection = Vec2( 0.f, -1.f );
		return true;
	}
	if ( g_engine->m_input->IsKeyDown( 'J' ) )
	{
		m_turretOrientationDegrees = GetTurnedTowardDegrees( m_turretOrientationDegrees, -180.f, 1.f );
		m_desiredTurretDirection = Vec2( -1.f, 0.f );
		return true;
	}
	if ( g_engine->m_input->IsKeyDown( 'L' ) )
	{
		m_turretOrientationDegrees = GetTurnedTowardDegrees( m_turretOrientationDegrees, 0.f, 1.f );
		m_desiredTurretDirection = Vec2( 1.f, 0.f );
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------------------------
bool Player::IsPlayer() const
{
	return true;
}

//------------------------------------------------------------------------------
void Player::InitializePlayerVerts()
{
	m_playerVerts[0] = Vertex( Vec3( -0.5f, -0.5f, 0.f ), Rgba8( 255, 255, 255, 255 ), Vec2( 0.f, 0.f ) );
	m_playerVerts[1] = Vertex( Vec3( 0.5f, -0.5f, 0.f ), Rgba8( 255, 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_playerVerts[2] = Vertex( Vec3( 0.5f, 0.5f, 0.f ), Rgba8( 255, 255, 255, 255 ), Vec2( 1.f, 1.f ) );
	m_playerVerts[3] = Vertex( Vec3( -0.5f, -0.5f, 0.f ), Rgba8( 255, 255, 255, 255 ), Vec2( 0.f, 0.f ) );
	m_playerVerts[4] = Vertex( Vec3( 0.5f, 0.5f, 0.f ), Rgba8( 255, 255, 255, 255 ), Vec2( 1.f, 1.f ) );
	m_playerVerts[5] = Vertex( Vec3( -0.5f, 0.5f, 0.f ), Rgba8( 255, 255, 255, 255 ), Vec2( 0.f, 1.f ) );
}

//-----------------------------------------------------------------------------------------------
void Player::InitializeTurretVerts()
{
	m_turretVerts[0] = Vertex( Vec3( -0.5f, -0.5f, 0.f ), Rgba8( 255, 255, 255, 255 ), Vec2( 0.f, 0.f ) );
	m_turretVerts[1] = Vertex( Vec3( 0.5f, -0.5f, 0.f ), Rgba8( 255, 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_turretVerts[2] = Vertex( Vec3( 0.5f, 0.5f, 0.f ), Rgba8( 255, 255, 255, 255 ), Vec2( 1.f, 1.f ) );
	m_turretVerts[3] = Vertex( Vec3( -0.5f, -0.5f, 0.f ), Rgba8( 255, 255, 255, 255 ), Vec2( 0.f, 0.f ) );
	m_turretVerts[4] = Vertex( Vec3( 0.5f, 0.5f, 0.f ), Rgba8( 255, 255, 255, 255 ), Vec2( 1.f, 1.f ) );
	m_turretVerts[5] = Vertex( Vec3( -0.5f, 0.5f, 0.f ), Rgba8( 255, 255, 255, 255 ), Vec2( 0.f, 1.f ) );
}

//------------------------------------------------------------------------------
void Player::UpdateFromKeyboard( [[maybe_unused]] float deltaSeconds )
{
	if ( g_engine->m_input->IsKeyDown( KEYCODE_F3 ) )
	{
		m_noClip = !m_noClip;
	}

	m_isMoving = PlayerControlKeyboard();
	TurretControlKeyboard();
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
		m_desiredMoveDirection = leftStickPos;
	}

	float rightStickMagnitude = controller.GetRightStick().GetMagnitude();
	if ( rightStickMagnitude > 0.f )
	{
		Vec2 rightStickPos = controller.GetRightStick().GetPosition();
		m_turretOrientationDegrees = GetTurnedTowardDegrees( m_turretOrientationDegrees, rightStickPos.GetOrientationDegrees(), 1.f );
		m_desiredTurretDirection = rightStickPos;
	}
}