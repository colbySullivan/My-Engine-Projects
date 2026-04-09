#pragma once

#include "Game/ActorHandle.hpp"

class Actor;
class Map;

//-----------------------------------------------------------------------------------------------
class Controller
{
public:
	Controller( Map* map );
	virtual ~Controller();

	virtual void Update( float deltaSeconds ) = 0;

	void Possess( Actor* actor );
	void Unpossess();
	Actor* GetActor() const;

	ActorHandle GetActorHandle() const { return m_actorHandle; }

protected:
	ActorHandle m_actorHandle;
	Map* m_map;
};