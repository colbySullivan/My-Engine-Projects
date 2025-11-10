#pragma once
#include "Game/Entity.hpp"


class Aries : public Entity
{
public:
	Aries( Game* owner, Vec2 const& startPos, float orientationDegrees, EntityFaction faction, Map* map, EntityType type );
	~Aries();

public:
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;

	void Shoot();
	void DriveForward( float deltaSeconds );

private:

};