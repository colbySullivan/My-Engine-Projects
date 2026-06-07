#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/SpriteAnimationDefinition.hpp"

//------------------------------------------------------------------------------
Player::Player(Game* owner, Vec2 const& startPos, float orientationDegrees, EntityFaction faction, Map* map, EntityType type )
	: Entity(owner, startPos, orientationDegrees, faction, map, type)
{
	m_physicsRadius = g_gameConfig->GetValue("playerPhysicsRadius", 0.3f);
	m_cosmeticRadius = g_gameConfig->GetValue("playerCosmeticRadius", 0.5f);
	m_isPushedByWalls = true;
	m_isPushedByEntities = true;
	m_doesPushEntities = true;
	m_isHitByBullets = true;
	m_health = PLAYER_HEALTH;
	m_lives = PLAYER_LIVES;
	m_bulletCooldown = 0.1f;
	InitializePlayerSpriteSheet();
	InitializePlayerVerts();
	InitializeTurretVerts();
	m_faction = faction;
	m_bodyTexture = m_game->m_playerBodyTexture;
	m_wizardTexture = m_game->m_playerTurretTexture;
	m_map = map;

	g_engine->m_render->BindTexture( nullptr );
}

//------------------------------------------------------------------------------
Player::~Player()
{

}

//------------------------------------------------------------------------------
void Player::Update([[maybe_unused]] float deltaSeconds)
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
	m_velocity = GetForwardNormal() * g_gameConfig->GetValue("playerSpeed", 1.0f);
	UpdateFromKeyboard(deltaSeconds);
	UpdateFromController(deltaSeconds);

	if ( m_lives <= 0 )
	{
		m_map->m_lostGame = true;
	}

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
	if ( m_health <= 0 )
	{		
		PlayDeathExplosion();
		return;
	}
	RenderPlayer();
	//RenderTurret();
}

//-----------------------------------------------------------------------------------------------
void Player::RenderPlayer() const
{
	if ( m_isDead )
	{
		return;
	}

	std::vector<Vertex> wizardVerts;

	float secondsPerFrame = m_currentAnimGroup->m_secondsPerFrame;
	const DirectionalAnimInfo& dirAnim = m_currentAnimGroup->m_directionalAnims[0];
	const SpriteAnimDefinition currentAnim = SpriteAnimDefinition( *m_currentSpriteSheet, dirAnim.startFrame, dirAnim.endFrame, secondsPerFrame, m_currentAnimGroup->m_playbackMode );

	const SpriteDefinition& wizardSprite = currentAnim.GetSpriteDefAtTime( m_frameTimeEntity );
	Vec2 wizardUVMins, wizardUVMaxs;
	wizardSprite.GetUVs( wizardUVMins, wizardUVMaxs );

	Vec2 mins( m_position.x - 0.5f, m_position.y - 0.5f );
	Vec2 maxs( m_position.x + 0.5f, m_position.y + 0.5f );
	AABB2 localBox( mins, maxs );

	AddVertsForAABB2D( wizardVerts, localBox, Rgba8( 255, 255, 255 ), wizardUVMins, wizardUVMaxs );

	g_engine->m_render->BindTexture( &m_currentSpriteSheet->GetTexture() );
	g_engine->m_render->DrawVertexArray( wizardVerts );
	g_engine->m_render->BindTexture( nullptr );
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
	g_engine->m_render->BindTexture( m_wizardTexture );
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

void Player::InitializePlayerSpriteSheet()
{
	const SpriteAnimationDefinition* spriteAnimDef = SpriteAnimationDefinition::GetByName( "Wizard" );
	if ( spriteAnimDef )
	{
		m_spriteAnimationDef = spriteAnimDef;
		m_currentAnimGroup = &spriteAnimDef->m_animationGroups[0];
		const char* spriteSheetPath = spriteAnimDef->m_spriteSheetPath.c_str();
		Texture* spriteSheetTexture = g_engine->m_render->CreateOrGetTextureFromFile( spriteSheetPath );
		m_currentSpriteSheet = new SpriteSheet( *spriteSheetTexture, spriteAnimDef->m_cellCount );
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
	m_isMoving = PlayerControlKeyboard();
	TurretControlKeyboard();
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