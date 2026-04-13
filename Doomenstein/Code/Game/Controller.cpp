#include "Game/Controller.hpp"
#include "Game/Actor.hpp"
#include "Game/Map.hpp"

//-----------------------------------------------------------------------------------------------
Controller::Controller( Map* map )
	: m_actorHandle( ActorHandle::INVALID )
	, m_map( map )
{
}

//-----------------------------------------------------------------------------------------------
Controller::~Controller()
{
	Unpossess();
}

//-----------------------------------------------------------------------------------------------
void Controller::Possess( Actor* actor )
{
	Unpossess();

	if ( actor )
	{
		m_actorHandle = actor->m_actorHandle;
		actor->OnPossessed( this );
	}
}

//-----------------------------------------------------------------------------------------------
void Controller::Unpossess()
{
	Actor* currentActor = GetActor();
	if ( currentActor )
	{
		currentActor->OnUnpossessed();
	}
	m_actorHandle = ActorHandle::INVALID;
}

//-----------------------------------------------------------------------------------------------
Actor* Controller::GetActor() const
{
	if ( !m_map )
		return nullptr;

	return m_map->GetActorByHandle( m_actorHandle );
}

//-----------------------------------------------------------------------------------------------
ActorHandle Controller::GetActorHandle() const
{
	return m_actorHandle;
}

//-----------------------------------------------------------------------------------------------
bool Controller::IsPlayerControlled() const
{
	return m_isCurrentlyPlayerControlled;
}
