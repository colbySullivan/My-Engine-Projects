#include "Game/PlayerController.hpp"
#include "Game/Actor.hpp"
#include "Game/Map.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"

//-----------------------------------------------------------------------------------------------
PlayerController::PlayerController( Map* map, Camera* camera )
	: Controller( map )
	, m_camera( camera )
	, m_freeFlyCameraPosition( 0.f, 0.f, 0.f )
	, m_freeFlyCameraOrientation( 0.f, 0.f, 0.f )
	, m_isFreeFlyMode( false )
{
	m_isCurrentlyPlayerControlled = true;
	m_hudTexture = g_engine->m_render->CreateTextureFromImage( "Data/Images/Hud_Base.png" );
	m_reticleTexture = g_engine->m_render->CreateTextureFromImage( "Data/Images/Reticle.png" );
}

//-----------------------------------------------------------------------------------------------
PlayerController::~PlayerController()
{
}

//-----------------------------------------------------------------------------------------------
void PlayerController::Update( float deltaSeconds )
{
	UpdateInput( deltaSeconds );
	UpdateCamera( deltaSeconds );
	g_engine->m_audio->UpdateListener( 
		m_controllerIndex, 
		WorldToFMOD( m_camera->GetPosition() ), 
		WorldToFMOD( m_camera->GetOrientation().GetForwardDir_IFwd_JLeft_KUp() ), 
		WorldToFMOD( Vec3::Z_AXIS ) 
	);
}

//-----------------------------------------------------------------------------------------------
void PlayerController::RenderUI() const
{
	if ( m_isFreeFlyMode )
	{
		return;
	}

	AABB2 worldViewport = m_camera->GetViewport();

	Camera uiCamera;
	float screenSizeX = g_gameConfig->GetValue( "screenSizeX", 1600.f );
	float screenSizeY = g_gameConfig->GetValue( "screenSizeY", 900.f );

	float viewportWidth = ( worldViewport.m_maxs.x - worldViewport.m_mins.x ) * screenSizeX;
	float viewportHeight = ( worldViewport.m_maxs.y - worldViewport.m_mins.y ) * screenSizeY;

	uiCamera.SetOrthographicView( Vec2( 0.f, 0.f ), Vec2( viewportWidth, viewportHeight ) );
	uiCamera.SetViewport( worldViewport );

	g_engine->m_render->BeginCamera( uiCamera );

	RenderWeaponUI( viewportWidth, viewportHeight );

	std::vector<Vertex> hudVerts;
	AddVertsForAABB2D( hudVerts, AABB2( Vec2( 0.f, 0.f ), Vec2( viewportWidth, viewportHeight * 0.15f ) ), Rgba8( 255, 255, 255 ) );
	g_engine->m_render->BindTexture( m_hudTexture );
	g_engine->m_render->DrawVertexArray( ( int )hudVerts.size(), hudVerts.data() );

	std::vector<Vertex> reticleVerts;
	AddVertsForAABB2D( reticleVerts, AABB2( Vec2( viewportWidth * 0.49f, viewportHeight * 0.49f ), Vec2( viewportWidth * 0.51f, viewportHeight * 0.51f ) ), Rgba8( 0, 255, 0 ) );
	g_engine->m_render->BindTexture( m_reticleTexture );
	g_engine->m_render->DrawVertexArray( ( int )reticleVerts.size(), reticleVerts.data() );

	Actor* ownerActor = GetActor();

	std::string healthHudText = Stringf( "%5.0f", ownerActor ? ownerActor->m_health : 0.f );
	std::vector<Vertex> textVerts;
	float textX = ( viewportHeight == screenSizeY ) ? ( viewportWidth * 0.2f ) : ( viewportWidth * 0.25f );
	AddVertsForTextTriangles2D( textVerts, healthHudText, Vec2( textX, 15.f ), viewportHeight * 0.08f, Rgba8( 255, 255, 255 ) );

	std::string killCountHudText = Stringf( "%5.0f", ownerActor ? m_killCount : 0.f );
	float killCountX = ( viewportHeight == screenSizeY ) ? ( viewportWidth * 0.01f ) : ( viewportWidth * 0.015f );
	AddVertsForTextTriangles2D( textVerts, killCountHudText, Vec2( 0.f, 15.f ), viewportHeight * 0.08f, Rgba8( 255, 255, 255 ) );

	std::string deathCountHudText = Stringf( "%5.0f", ownerActor ? m_deathCount : 0.f );
	float deathCountX = ( viewportHeight == screenSizeY ) ? ( viewportWidth * 0.01f ) : ( viewportWidth * 0.015f );
	AddVertsForTextTriangles2D( textVerts, deathCountHudText, Vec2( viewportWidth * 0.85f, 15.f ), viewportHeight * 0.08f, Rgba8( 255, 255, 255 ) );

	g_engine->m_render->BindTexture( nullptr );
	g_engine->m_render->DrawVertexArray( ( int )textVerts.size(), textVerts.data() );

	g_engine->m_render->EndCamera( uiCamera );
}

//-----------------------------------------------------------------------------------------------
void PlayerController::UpdateInput( float deltaSeconds )
{
	if ( m_isPlayerOne && g_engine->m_input->WasKeyJustPressed( 'F' ) && m_numOfPlayers == 1 )
	{
		ToggleCameraMode();
		m_isCurrentlyPlayerControlled = !m_isCurrentlyPlayerControlled;
		g_engine->m_input->EndFrame(); // Fixes issue multiple switches are called in a single frame
	}

	if ( m_isPlayerOne && g_engine->m_input->WasKeyJustPressed( 'N' ) && m_numOfPlayers == 1 )
	{
		m_isCurrentlyPlayerControlled = true;
		g_engine->m_input->EndFrame(); // Fixes issue multiple switches are called in a single frame
		PossessNextActor();
	}

	if ( m_isFreeFlyMode )
	{
		HandleFreeFlyInput( deltaSeconds );
	}
	else
	{
		Actor* actor = GetActor();
		if ( actor && !actor->m_isDead )
		{
			HandleActorInput( deltaSeconds );
		}
	}
}

//-----------------------------------------------------------------------------------------------
void PlayerController::HandleActorInput( float deltaSeconds )
{
	Actor* actor = GetActor();

	if ( !actor || actor->m_isDead )
	{
		return;
	}

	ProcessLookInput( deltaSeconds );
	ProcessMovementInput( deltaSeconds );
	ProcessWeaponChangeInput();
}

//-----------------------------------------------------------------------------------------------
void PlayerController::HandleFreeFlyInput( float deltaSeconds )
{

	if ( g_engine->m_console && ( g_engine->m_console->GetMode() == OPEN_FULL ) )
	{
		return;
	}

	XboxController const& controller = g_engine->m_input->GetController(m_controllerIndex);
	ProcessLookInput( deltaSeconds );

	float moveSpeed = FREE_FLY_SPEED;
	if ( g_engine->m_input->IsKeyDown( KEYCODE_SHIFT ) || controller.IsButtonDown( XboxButtonID::A ) )
	{
		moveSpeed = FREE_FLY_SPRINT_SPEED;
	}

	Vec2 leftStickPos = controller.GetLeftStick().GetPosition();
	float leftThreshold = controller.GetLeftStick().GetInnerDeadZoneFraction();

	Vec3 localMoveDir = Vec3( 0.f, 0.f, 0.f );
	if ( g_engine->m_input->IsKeyDown( 'W' ) || leftStickPos.y > leftThreshold )
	{
		localMoveDir.x += 1.f;
	}
	if ( g_engine->m_input->IsKeyDown( 'S' ) || leftStickPos.y < -leftThreshold )
	{
		localMoveDir.x -= 1.f;
	}
	if ( g_engine->m_input->IsKeyDown( 'A' ) || leftStickPos.x < -leftThreshold )
	{
		localMoveDir.y -= 1.f;
	}
	if ( g_engine->m_input->IsKeyDown( 'D' ) || leftStickPos.x > leftThreshold )
	{
		localMoveDir.y += 1.f;
	}
	if ( g_engine->m_input->IsKeyDown( 'Z' ) || controller.IsButtonDown( XboxButtonID::LEFT_BUMPER ) )
	{
		localMoveDir.z -= 1.f;
	}
	if ( g_engine->m_input->IsKeyDown( 'C' ) || controller.IsButtonDown( XboxButtonID::RIGHT_BUMPER ) )
	{
		localMoveDir.z += 1.f;
	}

	if ( localMoveDir.GetLength() > 0.f )
	{
		localMoveDir = localMoveDir.GetNormalized();

		Mat44 cameraMatrix = m_freeFlyCameraOrientation.GetAsMatrix_IFwd_JLeft_KUp();
		Vec3 forward = cameraMatrix.GetIBasis3D();
		Vec3 left = -cameraMatrix.GetJBasis3D();
		Vec3 up = Vec3::Z_AXIS;

		Vec3 worldMoveDir = forward * localMoveDir.x + left * localMoveDir.y + up * localMoveDir.z;
		worldMoveDir = worldMoveDir.GetNormalized();

		m_freeFlyCameraPosition += worldMoveDir * moveSpeed * deltaSeconds;
	}
}

//-----------------------------------------------------------------------------------------------
void PlayerController::ProcessMovementInput( [[maybe_unused]] float deltaSeconds )
{
	if ( g_engine->m_console && ( g_engine->m_console->GetMode() == OPEN_FULL ) )
	{
		return;
	}

	Actor* actor = GetActor();
	if ( !actor )
		return;

	XboxController const& controller = g_engine->m_input->GetController(m_controllerIndex);
	float rightTrigger = controller.GetRightTrigger();

	if ( ( m_isPlayerOne && g_engine->m_input->IsKeyDown( KEYCODE_LEFT_MOUSE ) ) || rightTrigger > 0.1f )
	{
		if ( actor && m_map && actor->CanFireWeapon() )
		{
			const WeaponDefinition* weaponDef = actor->GetCurrentWeapon();
			if ( weaponDef )
			{
				Vec3 raycastDir = GetRaycastDirection();

				if ( weaponDef->m_rayCount > 0 )
				{
					FireRaycastWeapon( actor, weaponDef, raycastDir );
				}
				else if ( weaponDef->m_projectileCount > 0 )
				{
					m_map->SpawnProjectileFromActor( actor, *weaponDef, raycastDir );
				}

				actor->FireWeapon();
			}
		}
	}

	float moveSpeed = actor->m_actorDef->m_walkSpeed;

	if ( g_engine->m_input->IsKeyDown( KEYCODE_SHIFT ) || controller.IsButtonDown( XboxButtonID::A ) )
	{
		moveSpeed = actor->m_actorDef->m_runSpeed;
	}

	Vec2 leftStickPos = controller.GetLeftStick().GetPosition();
	float leftThreshold = controller.GetLeftStick().GetInnerDeadZoneFraction();

	Vec3 localMoveDir = Vec3( 0.f, 0.f, 0.f );
	if ( ( m_isPlayerOne && g_engine->m_input->IsKeyDown( 'W' ) ) || leftStickPos.y > leftThreshold )
	{
		localMoveDir.x += 1.f;
	}
	if ( ( m_isPlayerOne && g_engine->m_input->IsKeyDown( 'S' ) ) || leftStickPos.y < -leftThreshold )
	{
		localMoveDir.x -= 1.f;
	}
	if ( ( m_isPlayerOne && g_engine->m_input->IsKeyDown( 'A' ) ) || leftStickPos.x < -leftThreshold )
	{
		localMoveDir.y += 1.f;
	}
	if ( ( m_isPlayerOne && g_engine->m_input->IsKeyDown( 'D' ) ) || leftStickPos.x > leftThreshold )
	{
		localMoveDir.y -= 1.f;
	}
	if ( ( m_isPlayerOne && g_engine->m_input->IsKeyDown( 'E' ) ) || controller.IsButtonDown( XboxButtonID::LEFT_BUMPER ) )
	{
		m_freeFlyCameraOrientation.m_rollDegrees = 15.f;
	}
	else if ( ( m_isPlayerOne && g_engine->m_input->IsKeyDown( 'Q' ) ) || controller.IsButtonDown( XboxButtonID::RIGHT_BUMPER ) )
	{
		m_freeFlyCameraOrientation.m_rollDegrees = -15.f;
	}
	else
	{
		m_freeFlyCameraOrientation.m_rollDegrees = 0.f;
	}

	if ( localMoveDir.GetLength() > 0.f )
	{
		localMoveDir = localMoveDir.GetNormalized();

		Mat44 cameraMatrix = m_freeFlyCameraOrientation.GetAsMatrix_IFwd_JLeft_KUp();
		Vec3 forward = cameraMatrix.GetIBasis3D();
		Vec3 left = cameraMatrix.GetJBasis3D();

		Vec3 worldMoveDir = forward * localMoveDir.x + left * localMoveDir.y;
		worldMoveDir.z = 0.f; 
		worldMoveDir = worldMoveDir.GetNormalized();

		actor->MoveInDirection( worldMoveDir, moveSpeed );
	}
}

//-----------------------------------------------------------------------------------------------
void PlayerController::ProcessLookInput( [[maybe_unused]] float deltaSeconds )
{
	XboxController const& controller = g_engine->m_input->GetController(m_controllerIndex);

	if ( m_isPlayerOne )
	{
		Vec2 mouseDelta = g_engine->m_input->GetCursorClientDelta();
		Vec2 lookInput = Vec2( static_cast< float >( mouseDelta.x ), static_cast< float >( mouseDelta.y ) );
		m_freeFlyCameraOrientation.m_yawDegrees -= lookInput.x * MOUSE_SENSITIVITY;
		m_freeFlyCameraOrientation.m_pitchDegrees += lookInput.y * MOUSE_SENSITIVITY;
	}

	Vec2 rightStickPos = controller.GetRightStick().GetPosition();
	float rightThreshold = controller.GetRightStick().GetInnerDeadZoneFraction();

	if ( rightStickPos.y > rightThreshold || rightStickPos.y < -rightThreshold )
	{
		m_freeFlyCameraOrientation.m_pitchDegrees -= rightStickPos.y * deltaSeconds * PITCH_RATE;
	}

	if ( rightStickPos.x > rightThreshold || rightStickPos.x < -rightThreshold )
	{
		m_freeFlyCameraOrientation.m_yawDegrees -= rightStickPos.x * deltaSeconds * YAW_RATE;
	}

	m_freeFlyCameraOrientation.m_pitchDegrees = GetClamped( m_freeFlyCameraOrientation.m_pitchDegrees, -MAX_CAMERA_PITCH, MAX_CAMERA_PITCH );

	Actor* actor = GetActor();
	if ( actor && !m_isFreeFlyMode )
	{
		actor->m_orientation.m_yawDegrees = m_freeFlyCameraOrientation.m_yawDegrees;
	}
}

//-----------------------------------------------------------------------------------------------
void PlayerController::UpdateCamera( [[maybe_unused]] float deltaSeconds )
{
	Actor* actor = GetActor();

	if ( actor && !m_isFreeFlyMode )
	{
		Vec3 cameraPos = actor->m_position;
		
		if ( actor->m_isDead )
		{
			cameraPos.z += actor->GetDeathCameraHeight();
			
			float deathPitch = -45.f;
			float pitchT = GetClamped( actor->m_deathAnimationTime / 1.0f, 0.f, 1.f );
			float currentPitch = Interpolate( m_freeFlyCameraOrientation.m_pitchDegrees, deathPitch, pitchT );
			EulerAngles deathCameraOrientation = m_freeFlyCameraOrientation;
			deathCameraOrientation.m_pitchDegrees = currentPitch;
			m_camera->SetPositionAndOrientation( cameraPos, deathCameraOrientation );
		}
		else
		{
			cameraPos.z += actor->m_actorDef->m_eyeHeight;
			m_camera->SetPositionAndOrientation( cameraPos, m_freeFlyCameraOrientation );
		}
	}
	else
	{
		m_camera->SetPositionAndOrientation( m_freeFlyCameraPosition, m_freeFlyCameraOrientation );
	}
}

//------------------------------------------------------------------------------
Camera* PlayerController::GetCamera() const
{
	return m_camera;
}

//------------------------------------------------------------------------------
Vec3 PlayerController::GetRaycastDirection() const
{
	Mat44 cameraMatrix = m_freeFlyCameraOrientation.GetAsMatrix_IFwd_JLeft_KUp();
	return cameraMatrix.GetIBasis3D();
}

//------------------------------------------------------------------------------
void PlayerController::FireRaycastWeapon( Actor* actor, const WeaponDefinition* weaponDef, const Vec3& direction )
{
	if ( !actor || !weaponDef || !m_map )
		return;


	Vec3 rayStart = actor->m_position;
	if ( actor->m_actorDef )
	{
		rayStart.z += actor->m_actorDef->m_eyeHeight;
	}

	Vec3 rayDirection = direction.GetNormalized();
	float rayRange = weaponDef->m_rayRange;

	RaycastResult3D result = m_map->RaycastAll( rayStart, rayDirection, rayRange, actor );

	//DebugAddWorldCylinder( rayStart, result.m_impactPos, 0.02f, 1.0f, Rgba8::BLUE, Rgba8::BLUE, DebugRenderMode::X_RAY ); 

	if ( result.m_didImpact )
	{
		RaycastResult3D worldHit = m_map->RaycastAll( rayStart, rayDirection, result.m_impactDist + 0.01f, actor );

		if ( worldHit.m_didImpact )
		{
			Actor* hitActor = m_map->FindActorAtPosition( result.m_impactPos );

			SpawnInfo spawnInfo;
			spawnInfo.m_name = hitActor ? "BloodSplatter" : weaponDef->m_projectileActorName;
			spawnInfo.m_spawnLocation = worldHit.m_impactPos;
			spawnInfo.m_actorOrientation = actor->m_orientation;

			Actor* projectile = m_map->SpawnActor( spawnInfo );

			if ( hitActor )
			{
				float damage = g_rng->RollRandomFloatInRange( weaponDef->m_rayDamage.m_min, weaponDef->m_rayDamage.m_max );
				hitActor->AttackedBy( actor, damage );
			}
		}
	}
}

//------------------------------------------------------------------------------
void PlayerController::RenderWeaponUI( float viewportWidth, float viewportHeight ) const
{
	Actor* ownerActor = GetActor();
	if ( !ownerActor )
	{
		return;
	}

	const WeaponDefinition* weapon = ownerActor->GetCurrentWeapon();
	if ( !weapon || !ownerActor->m_weaponSpriteSheet || !ownerActor->m_currentWeaponAnim )
	{
		return;
	}

	g_engine->m_render->SetBlendMode( BlendMode::ALPHA );
	g_engine->m_render->SetDepthMode( DepthMode::DISABLED );
	g_engine->m_render->BindShader( nullptr );

	float elapsedTime = ( float )( g_engine->m_systemClock->GetTotalSeconds() - ownerActor->m_weaponAnimStartTime );
	const SpriteDefinition& weaponSprite = ownerActor->m_currentWeaponAnim->GetSpriteDefAtTime( elapsedTime );

	Vec2 uvMins, uvMaxs;
	weaponSprite.GetUVs( uvMins, uvMaxs );

	const WeaponHUDDefinition& hud = weapon->m_hud;
	Vec2 weaponSize = hud.m_spriteSize;
	Vec2 pivot = hud.m_spritePivot;

	Vec2 scaledWeaponSize = m_map->m_game->m_numActivePlayers == 2 ? weaponSize * 0.5f : weaponSize;

	float weaponX = ( viewportWidth * 0.5f ) - ( scaledWeaponSize.x * pivot.x );
	float weaponY = ( viewportHeight * 0.15f ) + ( scaledWeaponSize.y * pivot.y );

	AABB2 weaponBounds( Vec2( weaponX, weaponY ), Vec2( weaponX + scaledWeaponSize.x, weaponY + scaledWeaponSize.y ) );

	std::vector<Vertex> weaponVerts;
	AddVertsForAABB2D( weaponVerts, weaponBounds, Rgba8::WHITE, uvMins, uvMaxs );

	g_engine->m_render->BindTexture( &ownerActor->m_weaponSpriteSheet->GetTexture() );
	g_engine->m_render->DrawVertexArray( ( int )weaponVerts.size(), weaponVerts.data() );
}

//------------------------------------------------------------------------------
Vec3 PlayerController::GetCameraPosition() const
{
	return m_freeFlyCameraPosition;
}

//------------------------------------------------------------------------------
void PlayerController::SetControllerIndex( int index, int numOfControllers )
{
	if ( numOfControllers == 1 && index == 1 )
	{
		m_controllerIndex = 0;
	}

	else if ( numOfControllers == 1 && index == 0 )
	{
		m_controllerIndex = -1;
	}
	else 
	{
		m_controllerIndex = index;
	}

	m_isPlayerOne = ( index == 0 );
	m_numOfPlayers = numOfControllers;
}

//------------------------------------------------------------------------------
int PlayerController::GetControllerIndex() const
{
	return m_controllerIndex;
}

//------------------------------------------------------------------------------
Camera* PlayerController::GetPlayerCamera() const
{
	return m_camera;
}

//-----------------------------------------------------------------------------------------------
void PlayerController::ToggleCameraMode()
{
	m_isFreeFlyMode = !m_isFreeFlyMode;

	if ( m_isFreeFlyMode && GetActor() )
	{
		Actor* actor = GetActor();
		m_freeFlyCameraPosition = actor->m_position;
		m_freeFlyCameraPosition.z += actor->m_actorDef->m_eyeHeight;
	}
	else if ( !m_isFreeFlyMode && GetActor() )
	{
		Actor* actor = GetActor();
		m_freeFlyCameraOrientation.m_yawDegrees = actor->m_orientation.m_yawDegrees;
		m_freeFlyCameraOrientation.m_pitchDegrees = 0.f;
	}
}

//-----------------------------------------------------------------------------------------------
void PlayerController::PossessNextActor()
{
	if ( m_map )
	{
		m_map->PossessNextActor( this );

		if ( GetActor() )
		{
			m_isFreeFlyMode = false;

			Actor* newActor = GetActor();
			m_freeFlyCameraOrientation.m_yawDegrees = newActor->m_orientation.m_yawDegrees;
			m_freeFlyCameraOrientation.m_pitchDegrees = 0.f;
			m_freeFlyCameraOrientation.m_rollDegrees = 0.f;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void PlayerController::ProcessWeaponChangeInput()
{
	Actor* actor = GetActor();
	if ( !actor )
		return;

	XboxController const& controller = g_engine->m_input->GetController(m_controllerIndex);

	if ( g_engine->m_input->WasKeyJustPressed( '1' ) || controller.WasButtonJustPressed( XboxButtonID::X ) )
	{
		actor->EquipWeapon( 0 );
	}
	if ( g_engine->m_input->WasKeyJustPressed( '2' ) || controller.WasButtonJustPressed( XboxButtonID::Y ))
	{
		actor->EquipWeapon( 1 );
	}
	if ( g_engine->m_input->WasKeyJustPressed( '3' ) )
	{
		actor->EquipWeapon( 2 );
	}

	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_LEFTARROW ) || controller.WasButtonJustPressed( XboxButtonID::DPAD_UP ) )
	{
		actor->EquipNextWeapon();
	}

	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_RIGHTARROW ) || controller.WasButtonJustPressed( XboxButtonID::DPAD_DOWN ) )
	{
		actor->EquipPreviousWeapon();
	}
}
