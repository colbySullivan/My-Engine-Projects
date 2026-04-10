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
	UpdateInput( deltaSeconds );
	UpdateCamera( deltaSeconds );

	if ( g_engine->m_input->WasKeyJustPressed( 'L' ) ) 
	{
		m_map->DebugPrintActors();
	}
}

//-----------------------------------------------------------------------------------------------
void PlayerController::UpdateInput( float deltaSeconds )
{
	InputSystem* input = g_engine->m_input;

	if ( input->WasKeyJustPressed( 'F' ) )
	{
		ToggleCameraMode();
		g_engine->m_console->AddLine( Rgba8( 255, 255, 0 ), Stringf( "Free Fly Mode: %s", m_isFreeFlyMode ? "TRUE" : "FALSE" ) );
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
	InputSystem* input = g_engine->m_input;
	ProcessLookInput( deltaSeconds );

	float moveSpeed = FREE_FLY_SPEED;
	if ( input->IsKeyDown( KEYCODE_SHIFT ) )
	{
		moveSpeed = FREE_FLY_SPRINT_SPEED;
	}

	Vec3 moveDir = Vec3( 0.f, 0.f, 0.f );

	if ( input->IsKeyDown( 'W' ) )		moveDir.x += 1.f;
	if ( input->IsKeyDown( 'S' ) )		moveDir.x -= 1.f;
	if ( input->IsKeyDown( 'A' ) )		moveDir.y -= 1.f;
	if ( input->IsKeyDown( 'D' ) )		moveDir.y += 1.f;
	if ( input->IsKeyDown( 'Z' ) )		moveDir.z -= 1.f;
	if ( input->IsKeyDown( 'C' ) )		moveDir.z += 1.f;

	if ( moveDir.GetLength() > 0.f )
	{
		moveDir = moveDir.GetNormalized();

		Mat44 cameraMatrix = m_freeFlyCameraOrientation.GetAsMatrix_IFwd_JLeft_KUp();
		Vec3 forward = cameraMatrix.GetIBasis3D();
		Vec3 left = -cameraMatrix.GetJBasis3D();
		Vec3 up = Vec3::Z_AXIS;

		Vec3 worldMoveDir = forward * moveDir.x + left * moveDir.y + up * moveDir.z;
		worldMoveDir = worldMoveDir.GetNormalized();

		m_freeFlyCameraPosition += worldMoveDir * moveSpeed * deltaSeconds;
	}
}

//-----------------------------------------------------------------------------------------------
void PlayerController::ProcessMovementInput( [[maybe_unused]] float deltaSeconds )
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

		Mat44 cameraMatrix = m_freeFlyCameraOrientation.GetAsMatrix_IFwd_JLeft_KUp();
		Vec3 forward = cameraMatrix.GetIBasis3D();
		Vec3 left = cameraMatrix.GetJBasis3D();

		Vec3 worldMoveDir = forward * moveDir.x + left * moveDir.y;
		worldMoveDir.z = 0.f; 
		worldMoveDir = worldMoveDir.GetNormalized();

		actor->MoveInDirection( worldMoveDir, moveSpeed );
	}
}

//-----------------------------------------------------------------------------------------------
void PlayerController::ProcessLookInput( [[maybe_unused]] float deltaSeconds )
{
	Vec2 mouseDelta = g_engine->m_input->GetCursorClientDelta();
	Vec2 lookInput = Vec2( static_cast< float >( mouseDelta.x ), static_cast< float >( mouseDelta.y ) );

	m_freeFlyCameraOrientation.m_yawDegrees -= lookInput.x * MOUSE_SENSITIVITY;
	m_freeFlyCameraOrientation.m_pitchDegrees += lookInput.y * MOUSE_SENSITIVITY;

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
		cameraPos.z += actor->m_actorDef->m_eyeHeight;

		m_camera->SetPositionAndOrientation( cameraPos, m_freeFlyCameraOrientation );

		if ( actor->m_isDead )
		{
			// #todo death camera behavior
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
Vec3 PlayerController::GetCameraPosition() const
{
	return m_freeFlyCameraPosition;
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