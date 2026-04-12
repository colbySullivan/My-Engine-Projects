#include "Game/AiController.hpp"
#include "Game/Actor.hpp"
#include "Engine/Core/Engine.hpp"

//-----------------------------------------------------------------------------------------------
AiController::AiController( Map* map, Camera* /*camera*/ )
	: Controller( map )
	, m_freeFlyCameraPosition( 0.f, 0.f, 0.f )
	, m_freeFlyCameraOrientation( 0.f, 0.f, 0.f )
	, m_orientation( 0.f, 0.f, 0.f )
{
}

//-----------------------------------------------------------------------------------------------
AiController::~AiController()
{
}

//-----------------------------------------------------------------------------------------------
void AiController::Update( float deltaSeconds )
{
	HandleMovement( deltaSeconds );
}

//-----------------------------------------------------------------------------------------------
void AiController::HandleMovement( [[maybe_unused]] float deltaSeconds )
{
	Actor* actor = GetActor();
	if ( !actor || actor->m_isDead || !actor->m_actorDef )
	{
		return;
	}

	Vec3 forward = actor->m_orientation.GetForwardDir_IFwd_JLeft_KUp();
	forward.z = 0.f;

	if ( forward.GetLength() > 0.f )
	{
		forward = forward.GetNormalized();

		actor->MoveInDirection( forward, actor->m_actorDef->m_walkSpeed );
	}
}

//-----------------------------------------------------------------------------------------------
Vec3 AiController::GetRaycastDirection() const
{
	Actor* actor = GetActor();
	if ( actor )
	{
		return actor->m_orientation.GetForwardDir_IFwd_JLeft_KUp();
	}

	return Vec3( 1.f, 0.f, 0.f );
}