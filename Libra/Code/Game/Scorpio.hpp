#pragma once
#include "Game/Entity.hpp"


class Scorpio : public Entity
{
public:
	Scorpio( Game* owner, Vec2 const& startPos, float orientationDegrees, EntityFaction faction, Map* map, EntityType type	 );
	~Scorpio();

public:
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;

	void Shoot();
	void DriveForward( float deltaSeconds );

private:

};