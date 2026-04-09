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
Actor::Actor( Game* owner, SpawnInfo spawnInfo )
	: m_game( owner )
	, m_position( spawnInfo.m_spawnLocation )
	, m_orientation( spawnInfo.m_actorOrientation ) // #todo rest of the variables
{
	if ( spawnInfo.m_name == "Marine" )			CreatePlayer();
	if ( spawnInfo.m_name == "Demon" )			CreateDemon();
	if ( spawnInfo.m_name == "SpawnPoint" )		CreateSpawnPoint();
}

Actor::Actor( ActorDefinition* ActorDef )
	: m_actorDef( ActorDef )
{
	Vec3 startZeroed = Vec3( 0.f, 0.f, 0.f );
	Vec3 endZeroed = Vec3( 0.f, 0.f, m_actorDef->m_physicsHeight );
	AddVertsForCylinder3D( m_vertexes, startZeroed, endZeroed, m_actorDef->m_physicsRadius, m_color, AABB2::ZERO_TO_ONE, 32 );
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
	////m_orientation.m_yawDegrees += m_angularVelocity.m_yawDegrees * deltaSeconds;
	//if ( m_controlledByPlayer && m_game->m_controlPlayerMode )
	//{
	//	UpdateMove();
	//}

	if ( m_actorDef && m_actorDef->m_simulated && !m_isDead )
	{
		UpdateMove();
	}

}

//-----------------------------------------------------------------------------------------------
void Actor::Render() const
{
	Mat44 modelToWorld = GetModelToWorldTransform();
	g_engine->m_render->BindShader(nullptr);
	g_engine->m_render->SetModelConstants( modelToWorld, m_color);
	g_engine->m_render->m_desiredBlendMode = BlendMode::OPAQUE;
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::WIREFRAME_CULL_BACK;
	g_engine->m_render->BindTexture( m_texture );
	g_engine->m_render->DrawVertexArray( ( int )m_vertexes.size(), m_vertexes.data() );

	Rgba8 modelColor = Rgba8(255,0,0);

	g_engine->m_render->SetModelConstants( modelToWorld, modelColor );
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	g_engine->m_render->DrawVertexArray( ( int )m_vertexes.size(), m_vertexes.data() );
}

//-----------------------------------------------------------------------------------------------
void Actor::UpdateMove()
{
	float deltaSeconds =  (float) g_engine->m_systemClock->GetDeltaSeconds();
	Vec3 dragForce = -m_velocity * m_actorDef->m_drag;
	m_acceleration += dragForce;
	m_velocity += m_acceleration * deltaSeconds;
	m_position += m_velocity * deltaSeconds;
	m_acceleration = Vec3( 0.f, 0.f, 0.f );
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

//-----------------------------------------------------------------------------------------------
void Actor::OnPossessed( Controller* newController )
{
	if ( m_currentController && m_currentController != newController )
	{
		m_savedAIController = m_currentController;
	}

	m_currentController = newController;
}

//-----------------------------------------------------------------------------------------------
void Actor::OnUnpossessed()
{
	m_currentController = nullptr;

	if ( m_savedAIController )
	{
		m_savedAIController->Possess( this );
		m_savedAIController = nullptr;
	}
}

//------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------
void Actor::CreatePlayer()
{
	m_actorDef = ActorDefinition::GetByName( "Marine" );
	m_height = m_actorDef->m_physicsHeight;
	m_radius = m_actorDef->m_physicsRadius;

	Vec3 startZeroed = Vec3( 0.f, 0.f, 0.f );
	m_height = m_actorDef->m_physicsHeight;
	Vec3 endZeroed = Vec3( 0.f, 0.f, m_actorDef->m_physicsHeight );
	AddVertsForCylinder3D( m_vertexes, startZeroed, endZeroed, m_actorDef->m_physicsRadius, m_color, AABB2::ZERO_TO_ONE, 32 );
}

//-----------------------------------------------------------------------------------------------
void Actor::CreateDemon()
{
	m_actorDef =  ActorDefinition::GetByName( "Demon");
	m_height = m_actorDef->m_physicsHeight;
	m_radius = m_actorDef->m_physicsRadius;
	Vec3 startZeroed = Vec3( 0.f, 0.f, 0.f );
	Vec3 endZeroed = Vec3( 0.f, 0.f, m_height );
	//Vec3 endZeroed = Vec3( 0.f, 0.f, m_actorDef->m_physicsHeight );
	AddVertsForCylinder3D( m_vertexes, startZeroed, endZeroed, m_actorDef->m_physicsRadius, m_color, AABB2::ZERO_TO_ONE, 32 );
}

//-----------------------------------------------------------------------------------------------
void Actor::CreateSpawnPoint()
{
	m_actorDef =  ActorDefinition::GetByName( "SpawnPoint" );
	Vec3 startZeroed = Vec3( 0.f, 0.f, 0.f );
	Vec3 endZeroed = Vec3( 0.f, 0.f, 1.0f );
	m_color = Rgba8( 255.f, 255.f, 0.f );
	m_height = 1.f;
	m_radius = 0.5;
	//AddVertsForCylinder3D( m_vertexes, startZeroed, endZeroed, m_radius, m_color, AABB2::ZERO_TO_ONE, 32 );
}

//-----------------------------------------------------------------------------------------------
void Actor::MoveInDirection( const Vec3& direction, float speed )
{
	if ( m_isDead || !m_actorDef )
		return;

	Vec3 moveForce = direction * speed * m_actorDef->m_drag;
	AddForce( moveForce );
}


//-----------------------------------------------------------------------------------------------
void Actor::AddForce( const Vec3& force )
{
	m_acceleration += force;
}