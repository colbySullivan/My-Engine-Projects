#pragma once
#include "Game/Entity.hpp"

class Bullet : public Entity
{
public:
	Bullet( Game* owner, Vec2 const& startPos, float orientationDegrees );
	~Bullet();

public:
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;
	void AddVertsForMe( std::vector<Vertex>& verts ) const;


	void Shoot();
	void DriveForward( float deltaSeconds );
	void Wander( float deltaSeconds );

private:
	float			m_bulletLifeTime;
	int				m_bouncesRemaining = 2;
	void CheckEntityCollisions();
};