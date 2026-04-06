#include "Game/Player.hpp"
#include "GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Clock.hpp"


//-----------------------------------------------------------------------------------------------
Player::Player( Game* owner )
{
	m_game = owner;
	PrintControlsToDevConsole();
	SetUpCamera();
}	

//------------------------------------------------------------------------------
Player::~Player()
{
	delete m_worldCamera;
	m_worldCamera = nullptr;
}

//------------------------------------------------------------------------------
void Player::Update( [[maybe_unused]] float deltaSeconds )
{
	XboxController const& controller = g_engine->m_input->GetController( 0 );

	UpdateKeyboardAndControllerInput( deltaSeconds, controller );

	m_worldCamera->SetPositionAndOrientation( m_position, m_orientation );
	
}

//------------------------------------------------------------------------------
void Player::Render() const
{
}

//------------------------------------------------------------------------------
void Player::UpdateKeyboardAndControllerInput( float deltaSeconds, XboxController const& controller )
{
	if ( g_engine->m_console && ( g_engine->m_console->GetMode() == OPEN_FULL ) )
	{
		return;
	}
	Vec2 leftStickPos = controller.GetLeftStick().GetPosition();
	Vec2 rightStickPos = controller.GetRightStick().GetPosition();
	float leftThreshold = controller.GetLeftStick().GetInnerDeadZoneFraction();
	float rightThreshold = controller.GetRightStick().GetInnerDeadZoneFraction();
	Vec2 cursorDelta = g_engine->m_input->GetCursorClientDelta();

	m_orientation.m_yawDegrees -= cursorDelta.x * MOUSE_SENSITIVITY;
	m_orientation.m_pitchDegrees += cursorDelta.y * MOUSE_SENSITIVITY;

	if ( rightStickPos.y > rightThreshold || rightStickPos.y < -rightThreshold )
	{
		m_orientation.m_pitchDegrees -= rightStickPos.y * deltaSeconds * PITCH_RATE;
	}

	if ( rightStickPos.x > rightThreshold || rightStickPos.x < -rightThreshold )
	{
		m_orientation.m_yawDegrees -= rightStickPos.x * deltaSeconds * YAW_RATE;
	}

	if ( g_engine->m_input->IsKeyDown( 'Q' ) || ( controller.GetLeftTrigger() != 0 ) )
	{
		m_orientation.m_rollDegrees -= ROLL_RATE * deltaSeconds;
	}
	if ( g_engine->m_input->IsKeyDown( 'E' ) || ( controller.GetRightTrigger() != 0 ) )
	{
		m_orientation.m_rollDegrees += ROLL_RATE * deltaSeconds;
	}
	float speed = MOVE_SPEED;
	if ( g_engine->m_input->IsKeyDown( KEYCODE_SHIFT ) || controller.IsButtonDown(XboxButtonID::A) )
	{
		speed *= 10.f;
	}
	//hudText2 = Stringf( "F8 to Randomize; ESDF = fly horizontal, AZ = fly vertical, space = lock raycast, hold t = slow, LMB = grab object" );
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
		localMoveDir.y += 1.f;
	}
	if ( g_engine->m_input->IsKeyDown( 'D' ) || leftStickPos.x > leftThreshold )
	{
		localMoveDir.y -= 1.f;
	}
	if ( g_engine->m_input->IsKeyDown( 'Z' ) || controller.IsButtonDown(XboxButtonID::LEFT_BUMPER) )
	{
		localMoveDir.z -= 1.f;
	}
	if ( g_engine->m_input->IsKeyDown( 'C' ) || controller.IsButtonDown(XboxButtonID::RIGHT_BUMPER) )
	{
		localMoveDir.z += 1.f;
	}

	ApplyMovement( localMoveDir, speed, deltaSeconds );

	if ( g_engine->m_input->IsKeyDown( 'H' ) || controller.WasButtonJustPressed(XboxButtonID::START) )
	{
		m_position = Vec3( 0.f, 0.f, 0.f );
		m_orientation = EulerAngles( 0.f, 0.f, 0.f );
	}
}

//------------------------------------------------------------------------------
void Player::PrintControlsToDevConsole()
{
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "=== CAMERA CONTROLS ===" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "Mouse X / Right Stick X  : Yaw" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "Mouse Y / Right Stick Y  : Pitch" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "Q / Left Trigger         : Roll left" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "E / Right Trigger        : Roll right" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "W / Left Stick Up        : Move forward" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "S / Left Stick Down      : Move backward" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "A / Left Stick Left      : Move left" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "D / Left Stick Right     : Move right" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "Z / Left Shoulder        : Move down" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "C / Right Shoulder       : Move up" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "Shift / A-Button         : Sprint" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "H / Start                : Reset position and orientation" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "P                        : Pause / unpause game clock" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "O                        : Single-step one frame" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "T                        : Toggle slow-motion mode" );
	g_engine->m_console->AddLine( DevConsole::INFO_MAJOR_COLOR, "================================" );
}

//------------------------------------------------------------------------------
void Player::ApplyMovement( Vec3 const& localMoveDir, float speed, float deltaSeconds )
{
	//m_orientation.m_pitchDegrees = GetClamped( m_orientation.m_pitchDegrees, -85.f, 85.f );
	//m_orientation.m_rollDegrees = GetClamped( m_orientation.m_rollDegrees, -45.f, 45.f );

	if ( localMoveDir == Vec3( 0.f, 0.f, 0.f ) )
	{
		return;
	}

	Mat44 rotationMatrix = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();

	Vec3 forward = rotationMatrix.GetIBasis3D();
	forward = Vec3( forward.x, forward.y, 0.f );
	Vec3 left = rotationMatrix.GetJBasis3D();
	Vec3 up = Vec3::Z_AXIS;

	Vec3 worldDir = forward * localMoveDir.x + left * localMoveDir.y + up * localMoveDir.z;
	worldDir = worldDir.GetNormalized();

	m_position += worldDir * speed * deltaSeconds;
}

//-----------------------------------------------------------------------------------------------
Mat44 Player::GetModelToWorldTransform() const
{
	Mat44 model = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
	model.SetTranslation3D( m_position );
	return model;
}

void Player::SetUpCamera()
{
	m_worldCamera = new Camera;
	float aspect = ( ( float )g_engine->m_window->GetClientDimensions().x / ( float )g_engine->m_window->GetClientDimensions().y );
	m_worldCamera->SetPerspectiveView( aspect, 60.f, 0.1f, 100.f );
	Mat44 cameraToRenderMatrix;
	cameraToRenderMatrix.SetIJK3D( Vec3( 0.f, 0.f, 1.f ), Vec3( -1.f, 0.f, 0.f ), Vec3( 0.f, 1.f, 0.f ) );
	m_worldCamera->SetCameraToRenderTransform( cameraToRenderMatrix );
}