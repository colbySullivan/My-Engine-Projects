#pragma once
#include "Game/Entity.hpp"

class Leo : public Entity
{
public:
	Leo( Game* owner, Vec2 const& startPos, float orientationDegrees, EntityFaction faction );
	~Leo();

public:
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;

	void DriveForward( float deltaSeconds );
	void Wander( float deltaSeconds );

private:

};