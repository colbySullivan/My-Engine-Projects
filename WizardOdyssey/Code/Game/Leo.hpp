#pragma once
#include "Game/Entity.hpp"

class Leo : public Entity
{
public:
	Leo( Game* owner, Vec2 const& startPos, float orientationDegrees, EntityFaction faction, Map* map, EntityType type );
	~Leo();

public:
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;

	void Respawn() override;
	void Wander( float deltaSeconds );

private:

};