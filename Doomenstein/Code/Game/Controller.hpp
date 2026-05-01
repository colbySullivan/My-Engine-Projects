#pragma once

#include "Game/ActorHandle.hpp"
#include "Engine/Renderer/Camera.hpp"

class Actor;
class Map;

//-----------------------------------------------------------------------------------------------
class Controller
{
public:
	Controller( Map* map );
	virtual ~Controller();

	virtual void Update( float deltaSeconds ) = 0;
	virtual void RenderUI() const = 0;
	virtual Camera* GetPlayerCamera() const { return nullptr; }
	void Possess( Actor* actor );
	void Unpossess();
	Actor* GetActor() const;

	ActorHandle GetActorHandle() const;
	virtual void DamagedBy( Actor* attacker, float damage );
	bool IsPlayerControlled() const;
	ActorHandle GetTargetActorHandle() const;

protected:
	ActorHandle m_actorHandle;
	Map* m_map;
	bool m_isCurrentlyPlayerControlled = false;
	ActorHandle m_targetActorHandle;
};