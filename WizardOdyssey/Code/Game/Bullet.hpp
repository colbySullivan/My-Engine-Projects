#pragma once
#include "Game/Entity.hpp"

class Bullet : public Entity
{
public:
	Bullet( Game* owner, Vec2 const& startPos, float orientationDegrees, EntityFaction faction, Map* map, EntityType type, std::string defName );
	~Bullet();

public:
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;
	void AddVertsForMe( std::vector<Vertex>& verts ) const;
	void Die() override;

	void Respawn() override;
	void Wander( float deltaSeconds );

private:
	Timer*			m_bulletTimer = nullptr;
	float			m_bulletLifeTime;
	int				m_bouncesRemaining = 0;
	void CheckEntityCollisions();
	void SpawnAndShoot( float deltaSeconds );
};