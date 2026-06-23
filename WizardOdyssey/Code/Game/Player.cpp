#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/SpriteAnimationDefinition.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/DebugRender.hpp"

//------------------------------------------------------------------------------
Player::Player( Game* owner, Vec2 const& startPos, float orientationDegrees, EntityFaction faction, Map* map, EntityType type )
	: Entity( owner, startPos, orientationDegrees, faction, map, type )
{
	m_bulletCooldown = 0.7f; // #todo expose this!
	m_defName = m_game->m_playerDefChosen;
	InitializeDefitionStats();
	InitializeSpriteSheet();
	InitializePlayerVerts();
	InitializeTurretVerts();
	m_faction = faction;
	//m_gunTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/goldgun.png" );
	m_wizardTexture = m_game->m_playerTurretTexture;
	m_map = map;
	if ( !m_hitTimer )
	{
		m_hitTimer = new Timer( 2.2f );
	}
	else
	{
		delete m_hitTimer;
		m_hitTimer = new Timer( 2.2f );
	}
	m_hitTimer->Start();
}

//------------------------------------------------------------------------------
Player::~Player()
{

}

//------------------------------------------------------------------------------
void Player::Update( [[maybe_unused]] float deltaSeconds )
{
	m_frameTimeEntity += deltaSeconds;
	if ( m_health <= 0 )
	{
		if ( !m_justDied )
		{
			m_frameTimeEntity = 0.0f;
			m_justDied = true;
		}
		return;
	}

	m_isMoving = false;
	m_velocity = Vec2( 0.f, 0.f );

	UpdateFromKeyboard( deltaSeconds );
	UpdateFromController( deltaSeconds );
	UpdateTurretOrientation();

	if ( m_lives <= 0 )
	{
		m_map->m_lostGame = true;
	}

	//TryShoot( m_turretOrientationDegrees, deltaSeconds, m_faction );
	for ( int i = 0; i < MAX_WEAPONS; i++ )
	{
		if ( m_weapons[i] )
			m_weapons[i]->Update( deltaSeconds, m_position, m_turretOrientationDegrees, m_faction, m_map, this, m_numEquipedWeapons );
	}

	m_position += m_velocity * m_walkSpeed * deltaSeconds;
}

//------------------------------------------------------------------------------
void Player::Render() const
{
	if ( m_health <= 0 )
	{
		PlayDeathExplosion();
		return;
	}
	Entity::Render();
	std::string hudText = Stringf( "Player health: %i", m_health );
	DebugAddScreenText( hudText, AABB2( Vec2( 0.f, 0.f ), Vec2( 1600.f, 800.f ) ), 15.f, Vec2( 0.f, 1.f ), 0.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 255, 255 ) );
	//RenderGun();
	for ( int i = 0; i < MAX_WEAPONS; i++ )
	{
		if ( m_weapons[i] )
			m_weapons[i]->Render( m_position, m_game->m_mouseWorldWindowPosition, m_numEquipedWeapons );
	}
}

//-----------------------------------------------------------------------------------------------
void Player::RenderGun() const
{
	if ( m_isDead )
		return;
	Vertex tempTurretWorldVerts[NUM_TURRET_VERTS];

	for ( int vertIndex = 0; vertIndex < NUM_TURRET_VERTS; ++vertIndex )
	{
		tempTurretWorldVerts[vertIndex] = m_turretVerts[vertIndex];
	}

	TransformVertexArrayXY3D( NUM_PLAYER_VERTS, tempTurretWorldVerts, 0.5f, m_turretOrientationDegrees, m_position );
	g_engine->m_render->BindTexture( m_gunTexture );
	g_engine->m_render->DrawVertexArray( NUM_TURRET_VERTS, tempTurretWorldVerts );
	g_engine->m_render->BindTexture( nullptr );

	if ( m_game->g_drawDebug )
	{
		DebugRender();
	}
}

//-----------------------------------------------------------------------------------------------
void Player::UpdateLinearMovement()
{
	Vec2 moveDirection = Vec2( 0.f, 0.f );
	float playerSpeed = m_walkSpeed * m_actorDef->m_gameStats.m_speedBoost;

	if ( g_engine->m_input->IsKeyDown( 'W' ) || g_engine->m_input->IsKeyDown( 'E' ) )
	{
		moveDirection.y += 1.0f;
		m_isMoving = true;
	}
	if ( g_engine->m_input->IsKeyDown( 'S' ) )
	{
		moveDirection.y -= 1.0f;
		m_isMoving = true;
	}
	if ( g_engine->m_input->IsKeyDown( 'A' ) )
	{
		moveDirection.x -= 1.0f;
		m_isMoving = true;
	}
	if ( g_engine->m_input->IsKeyDown( 'D' ) || g_engine->m_input->IsKeyDown( 'F' ) )
	{
		moveDirection.x += 1.0f;
		m_isMoving = true;
	}

	if ( moveDirection.GetLengthSquared() > 0.0f )
	{
		moveDirection.Normalize();
		m_velocity = moveDirection * playerSpeed;
	}
}

//-----------------------------------------------------------------------------------------------
void Player::UpdateTurretOrientation()
{
	Vec2 directionToMouse = m_game->m_mouseWorldWindowPosition - m_position;

	if ( directionToMouse.GetLengthSquared() > 0.0f )
	{
		m_turretOrientationDegrees = directionToMouse.GetOrientationDegrees();
	}
}

//-----------------------------------------------------------------------------------------------
bool Player::IsPlayer() const
{
	return true;
}

//------------------------------------------------------------------------------
void Player::InitializePlayerVerts()
{
	AddVertsForAABB2D( m_playerVerts, *m_bodyABB2, Rgba8( 255, 255, 255 ) );
}

//-----------------------------------------------------------------------------------------------
void Player::InitializeTurretVerts()
{
	AddVertsForAABB2D( m_turretVerts, *m_turretABB2, Rgba8( 255, 255, 255 ) );
}

//------------------------------------------------------------------------------
void Player::UpdateFromKeyboard( [[maybe_unused]] float deltaSeconds )
{
	UpdateLinearMovement();
}

//------------------------------------------------------------------------------
void Player::Respawn()
{
	if ( m_lives > 1 )
	{
		m_isDead = false;
	}
	m_lives -= 1;
	m_health = PLAYER_HEALTH;
}

//-----------------------------------------------------------------------------------------------
void Player::PlayerHit()
{
	if ( m_hitTimer && m_hitTimer->DecrementPeriodIfElapsed() )
	{
		m_hitTimer->Start();
		TakeDamage();
	}
}

//-----------------------------------------------------------------------------------------------
void Player::AddWeapon( std::string weaponName )
{
	const WeaponDefinition* newWeaponDef = WeaponDefinition::GetByName( weaponName );
	if ( newWeaponDef )
	{
		this->m_weapons[m_numEquipedWeapons] = new Weapon( newWeaponDef, m_numEquipedWeapons );
		this->m_numEquipedWeapons++;
	}	
}
//------------------------------------------------------------------------------
void Player::UpdateFromController( [[maybe_unused]] float deltaSeconds )
{
	XboxController const& controller = g_engine->m_input->GetController( 0 );
	float playerSpeed = g_gameConfig->GetValue( "playerSpeed", 1.0f );

	float leftStickMagnitude = controller.GetLeftStick().GetMagnitude();
	if ( leftStickMagnitude > 0.f )
	{
		m_isMoving = true;
		Vec2 leftStickPos = controller.GetLeftStick().GetPosition();
		m_velocity = leftStickPos * playerSpeed;
	}

	float rightStickMagnitude = controller.GetRightStick().GetMagnitude();
	if ( rightStickMagnitude > 0.f )
	{
		Vec2 rightStickPos = controller.GetRightStick().GetPosition();
		m_turretOrientationDegrees = rightStickPos.GetOrientationDegrees();
	}
}

//-----------------------------------------------------------------------------------------------
void Player::SetCurrentAnimGroup( const std::string& groupName )
{
	if ( !m_spriteAnimationDef )
	{
		return;
	}

	for ( int i = 0; i < m_spriteAnimationDef->m_animationGroups.size(); ++i )
	{
		if ( m_spriteAnimationDef->m_animationGroups[i].m_name == groupName )
		{
			m_currentAnimGroup = &m_spriteAnimationDef->m_animationGroups[i];
			m_currentAnimStartTime = g_engine->m_systemClock->GetTotalSeconds();

			if ( !m_currentAnimGroup->m_directionalAnims.empty() )
			{
				const DirectionalAnimInfo& dirAnim = m_currentAnimGroup->m_directionalAnims[0];
				int startFrame = dirAnim.startFrame;
				int endFrame = dirAnim.endFrame;
				float animDuration = ( endFrame - startFrame + 1 ) * m_currentAnimGroup->m_secondsPerFrame;

				if ( !m_animTimer )
				{
					m_animTimer = new Timer( animDuration );
				}
				else
				{
					delete m_animTimer;
					m_animTimer = new Timer( animDuration );
				}
				m_animTimer->Start();
			}

			return;
		}
	}
}