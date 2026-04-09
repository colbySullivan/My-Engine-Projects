#include "Game/PlayerController.hpp"
#include "Game/Actor.hpp"
#include "Game/Map.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"

//-----------------------------------------------------------------------------------------------
PlayerController::PlayerController( Map* map, Camera* camera )
	: Controller( map )
	, m_camera( camera )
	, m_freeFlyCameraPosition( 0.f, 0.f, 0.f )
	, m_freeFlyCameraOrientation( 0.f, 0.f, 0.f )
	, m_isFreeFlyMode( false )
{
}

//-----------------------------------------------------------------------------------------------
PlayerController::~PlayerController()
{
}

//-----------------------------------------------------------------------------------------------
void PlayerController::Update( float deltaSeconds )
{
	InputSystem* input = g_engine->m_input;
	UpdateInput( deltaSeconds );
	UpdateCamera( deltaSeconds );

	Actor* actor = GetActor();
}

//-----------------------------------------------------------------------------------------------
void PlayerController::UpdateInput( float deltaSeconds )
{
	InputSystem* input = g_engine->m_input;

	if ( input->WasKeyJustPressed( 'F' ) )
	{
		ToggleCameraMode();
	}

	if ( input->WasKeyJustPressed( 'N' ) )
	{
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
	ProcessLookInput( deltaSeconds );
	ProcessMovementInput( deltaSeconds );
}

//-----------------------------------------------------------------------------------------------
void PlayerController::HandleFreeFlyInput( float deltaSeconds )
{
	ProcessLookInput( deltaSeconds );

	float moveSpeed = FREE_FLY_SPEED;
	if ( g_engine->m_input->IsKeyDown( KEYCODE_SHIFT ) )
	{
		moveSpeed = FREE_FLY_SPRINT_SPEED;
	}

	Vec3 moveDir = Vec3( 0.f, 0.f, 0.f );

	if (  g_engine->m_input->IsKeyDown( 'W' ) )		moveDir.x += 1.f;
	if (  g_engine->m_input->IsKeyDown( 'S' ) )		moveDir.x -= 1.f;
	if (  g_engine->m_input->IsKeyDown( 'A' ) )		moveDir.y += 1.f;
	if (  g_engine->m_input->IsKeyDown( 'D' ) )		moveDir.y -= 1.f;
	if (  g_engine->m_input->IsKeyDown( 'Z' ) )		moveDir.z -= 1.f;
	if (  g_engine->m_input->IsKeyDown( 'C' ) )		moveDir.z += 1.f;

	if ( moveDir.GetLength() > 0.f )
	{
		moveDir = moveDir.GetNormalized();
		m_freeFlyCameraPosition += moveDir * moveSpeed * deltaSeconds;
	}
}

//-----------------------------------------------------------------------------------------------
void PlayerController::ProcessMovementInput( float deltaSeconds )
{
	Actor* actor = GetActor();
	if ( !actor )
		return;

	float moveSpeed = actor->m_actorDef->m_walkSpeed;

	if ( g_engine->m_input->IsKeyDown( KEYCODE_SHIFT ) )
	{
		moveSpeed = actor->m_actorDef->m_runSpeed;
	}

	Vec3 moveDir = Vec3( 0.f, 0.f, 0.f );

	if ( g_engine->m_input->IsKeyDown( 'W' ) )		moveDir.x += 1.f;
	if ( g_engine->m_input->IsKeyDown( 'S' ) )		moveDir.x -= 1.f;
	if ( g_engine->m_input->IsKeyDown( 'A' ) )		moveDir.y += 1.f;
	if ( g_engine->m_input->IsKeyDown( 'D' ) )		moveDir.y -= 1.f;

	if ( moveDir.GetLength() > 0.f )
	{
		moveDir = moveDir.GetNormalized();

		Mat44 cameraMatrix = actor->m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
		Vec3 forward = cameraMatrix.GetIBasis3D();
		Vec3 left = cameraMatrix.GetJBasis3D();

		Vec3 worldMoveDir = forward * moveDir.x + left * moveDir.y;
		worldMoveDir.z = 0.f; 
		worldMoveDir = worldMoveDir.GetNormalized();

		actor->MoveInDirection( worldMoveDir, moveSpeed );
	}
}

//-----------------------------------------------------------------------------------------------
void PlayerController::ProcessLookInput( float deltaSeconds )
{
	Vec2 mouseDelta = g_engine->m_input->GetCursorClientDelta();
	Vec2 lookInput = Vec2( static_cast< float >( mouseDelta.x ), static_cast< float >( mouseDelta.y ) );

	m_freeFlyCameraOrientation.m_yawDegrees += lookInput.x * MOUSE_SENSITIVITY;
	m_freeFlyCameraOrientation.m_pitchDegrees -= lookInput.y * MOUSE_SENSITIVITY;

	m_freeFlyCameraOrientation.m_pitchDegrees = GetClamped( m_freeFlyCameraOrientation.m_pitchDegrees, -MAX_CAMERA_PITCH, MAX_CAMERA_PITCH );

	Actor* actor = GetActor();
	if ( actor && !m_isFreeFlyMode )
	{
		actor->m_orientation.m_yawDegrees = m_freeFlyCameraOrientation.m_yawDegrees;
	}
}

//-----------------------------------------------------------------------------------------------
void PlayerController::UpdateCamera( float deltaSeconds )
{
	Actor* actor = GetActor();

	if ( actor && !m_isFreeFlyMode )
	{
		Vec3 cameraPos = actor->m_position;
		cameraPos.z += actor->m_actorDef->m_eyeHeight;

		m_camera->SetPosition( cameraPos );

		if ( actor->m_isDead )
		{
			// #todo death camera behavior
		}
	}
	// #Todo free fly camera
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
		}
	}
}