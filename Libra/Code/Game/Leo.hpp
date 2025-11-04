#pragma once
#include "Game/Entity.hpp"

class Leo : Entity
{
public:
	Leo( Game* owner, Vec2 const& startPos );
	~Leo();

public:
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;

	void Shoot();
	void DriveForward( float deltaSeconds );

private:

};