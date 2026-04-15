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
	MoveToPlayer( deltaSeconds );
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
		return actor->m_orientation.GetForwardDir_IFwd_JLeft_KUp().GetNormalized();
	}

	return Vec3( 1.f, 0.f, 0.f );
}

//-----------------------------------------------------------------------------------------------
void AiController::DamagedBy( Actor* attacker, [[maybe_unused]] float damage )
{
	if ( attacker )
	{
		m_targetActorHandle = attacker->m_actorHandle;
	}
}

RaycastResult3D AiController::RaycastSearchForPlayer( const Vec3& start, const Vec3& direction, float distance, Actor* owner )
{
	Actor* actor = GetActor();
	RaycastResult3D closestResult;
	closestResult.m_impactDist = distance;

	float sightRadius = actor->m_actorDef->m_sightRadius;

	for ( int actorIndex = 0; actorIndex < (int)m_map->m_actorVector.size(); ++actorIndex )
	{
		Actor* otherActor = m_map->m_actorVector[actorIndex];
		if ( !otherActor || otherActor == owner || otherActor->m_isDead )
		{
			continue;
		}

		Vec3 displacementToOther = otherActor->m_position - actor->m_position;
		float distToOther = displacementToOther.GetLength();
		if ( distToOther > sightRadius )
		{
			continue;
		}

		Vec3 forwardDir = direction.GetNormalized();
		Vec3 otherActorEnd = otherActor->m_position;
		otherActorEnd.z += otherActor->m_height;

		RaycastResult3D currentActorHit = RaycastVsCylinder( start, forwardDir, sightRadius, otherActor->m_position, otherActorEnd, otherActor->m_radius );

		if ( currentActorHit.m_didImpact && currentActorHit.m_impactDist < closestResult.m_impactDist )
		{
			closestResult = currentActorHit;
			m_targetActorHandle = otherActor->m_actorHandle;
		}
	}
	return closestResult;
}

void AiController::MoveToPlayer( float deltaSeconds )
{
	Actor* actor = GetActor();
	if ( !actor )
	{
		return;
	}
	RaycastResult3D result;
	Actor* targetActor = m_map->GetActorByHandle( m_targetActorHandle );
	if ( !targetActor || targetActor->m_isDead )
	{
		m_targetActorHandle = ActorHandle();

		Vec3 forward = actor->m_orientation.GetForwardDir_IFwd_JLeft_KUp();
		result = RaycastSearchForPlayer( actor->m_position + Vec3( 0.f, 0.f, 0.1f ), forward.GetNormalized(), 10.f, actor );
		if ( result.m_didImpact )
		{
			Actor* hitActor = m_map->GetActorByHandle( m_targetActorHandle );
			if ( hitActor && hitActor->m_controlledByPlayer )
			{
				m_targetActorHandle = hitActor->m_actorHandle;
				targetActor = hitActor;
			}
		}
	}

	if ( m_targetActorHandle.IsValid() && targetActor && !targetActor->m_isDead )
	{
		Vec3 displacementToTarget = targetActor->m_position - actor->m_position;
		float distanceToTarget = displacementToTarget.GetLength();
		float desiredYaw = Atan2Degrees( displacementToTarget.y, displacementToTarget.x );
		float maxTurnDegrees = actor->m_actorDef->m_turnSpeed * deltaSeconds;
		actor->m_orientation.m_yawDegrees = GetTurnedTowardDegrees( actor->m_orientation.m_yawDegrees, desiredYaw, maxTurnDegrees );
		
		const WeaponDefinition* weapon = actor->GetCurrentWeapon();
		if ( weapon && weapon->m_meleeCount > 0 )
		{
			if ( distanceToTarget <= weapon->m_meleeRange && actor->CanFireWeapon() )
			{
				//targetActor->AttackedBy( actor, g_rng.RollRandomFloatInRange( weapon->m_meleeDamage.m_min, weapon->m_meleeDamage.m_max ) );
				targetActor->AttackedBy( actor,weapon->m_meleeDamage.m_min );
				actor->FireWeapon();
			}
			else
			{
				HandleMovement( deltaSeconds );
			}
		}
		else
		{
			HandleMovement( deltaSeconds );
		}
	}
}
