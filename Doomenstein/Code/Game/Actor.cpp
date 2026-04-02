#include "Game/Actor.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Game/Player.hpp"

//-----------------------------------------------------------------------------------------------
Actor::Actor( Game* owner, Vec3 start, Vec3 end, float radius, int numSlices )
	: m_radius( radius )
	, m_game( owner )
	, m_position( start )
	, m_start( start )
	, m_end( end )
{
	m_height = m_end.z - m_start.z;
	Vec3 startZeroed = Vec3( 0.f, 0.f, 0.f );
	Vec3 endZeroed = Vec3( 0.f, 0.f, m_height );
	AddVertsForCylinder3D( m_vertexes, startZeroed, endZeroed, radius, m_color, AABB2::ZERO_TO_ONE, numSlices );
}

//-----------------------------------------------------------------------------------------------
Actor::~Actor()
{

}

//-----------------------------------------------------------------------------------------------
void Actor::Update( [[maybe_unused]] float deltaSeconds )
{
	//m_orientation.m_pitchDegrees += m_angularVelocity.m_pitchDegrees * deltaSeconds;
	//m_orientation.m_rollDegrees += m_angularVelocity.m_rollDegrees * deltaSeconds;
	//m_orientation.m_yawDegrees += m_angularVelocity.m_yawDegrees * deltaSeconds;
	if ( m_controlledByPlayer && m_game->m_controlPlayerMode )
	{
		UpdateMove();
	}
}

//-----------------------------------------------------------------------------------------------
void Actor::Render() const
{
	Mat44 modelToWorld = GetModelToWorldTransform();
	g_engine->m_render->BindShader(nullptr);
	g_engine->m_render->SetModelConstants( modelToWorld, Rgba8(255, 255, 255));
	g_engine->m_render->m_desiredBlendMode = BlendMode::OPAQUE;
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::WIREFRAME_CULL_BACK;
	g_engine->m_render->BindTexture( m_texture );
	g_engine->m_render->DrawVertexArray( ( int )m_vertexes.size(), m_vertexes.data() );

	Rgba8 modelColor = Rgba8(255,0,0);
	if ( m_actorType == PROJECTILE )
	{
		modelColor = Rgba8( 0, 0, 255 );
	}

	g_engine->m_render->SetModelConstants( modelToWorld, modelColor );
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	g_engine->m_render->DrawVertexArray( ( int )m_vertexes.size(), m_vertexes.data() );
}

//-----------------------------------------------------------------------------------------------
void Actor::UpdateMove()
{
	float deltaSeconds =  (float) g_engine->m_systemClock->GetDeltaSeconds();
	XboxController const& controller = g_engine->m_input->GetController( 0 );

	if ( g_engine->m_console && ( g_engine->m_console->GetMode() == OPEN_FULL ) )
	{
		return;
	}
	Vec2 leftStickPos = controller.GetLeftStick().GetPosition();
	Vec2 rightStickPos = controller.GetRightStick().GetPosition();
	float leftThreshold = controller.GetLeftStick().GetInnerDeadZoneFraction();
	float rightThreshold = controller.GetRightStick().GetInnerDeadZoneFraction();

	if ( rightStickPos.y > rightThreshold || rightStickPos.y < -rightThreshold )
	{
		m_orientation.m_pitchDegrees -= rightStickPos.y * deltaSeconds * PITCH_RATE;
	}

	if ( rightStickPos.x > rightThreshold || rightStickPos.x < -rightThreshold )
	{
		m_orientation.m_yawDegrees -= rightStickPos.x * deltaSeconds * YAW_RATE;
	}
	float speed = MOVE_SPEED;
	if ( g_engine->m_input->IsKeyDown( KEYCODE_SHIFT ) || controller.IsButtonDown( XboxButtonID::A ) )
	{
		speed *= 10.f;
	}

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

	ApplyMovement( localMoveDir, speed, deltaSeconds );
}

//------------------------------------------------------------------------------
void Actor::SetPosXY( float x, float y )
{
	m_position.x = x;
	m_position.y = y;
}

//------------------------------------------------------------------------------
Mat44 Actor::GetModelToWorldTransform() const
{
	Mat44 model = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
	model.SetTranslation3D( m_position );
	return model;
}

void Actor::ApplyMovement( Vec3 localMoveDir, float speed, float deltaSeconds )
{
	if ( localMoveDir == Vec3( 0.f, 0.f, 0.f ) )
	{
		return;
	}

	Mat44 rotationMatrix = m_game->m_player->m_orientation.GetAsMatrix_IFwd_JLeft_KUp();

	Vec3 forward = rotationMatrix.GetIBasis3D();
	forward.z = 0.f;
	Vec3 left = rotationMatrix.GetJBasis3D();
	Vec3 up = rotationMatrix.GetKBasis3D();

	Vec3 worldDir = forward * localMoveDir.x + left * localMoveDir.y + up * localMoveDir.z;
	worldDir = worldDir.GetNormalized();

	m_position += worldDir * speed * deltaSeconds;
}
