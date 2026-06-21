#pragma once
#include "Game/Entity.hpp"


class Enemy : public Entity
{
public:
	Enemy( Game* owner, Vec2 const& startPos, float orientationDegrees, EntityFaction faction, Map* map, EntityType type, std::string defName );
	~Enemy();

public:
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;
	virtual bool TakeDamage(int incomingDamage = 1, bool isMagic = false);

	void Respawn() override;

private:
	Timer* m_spawnTimer;
	Texture* m_spawnTexture;
	void UpdateMoveIfSpawned();
	void RenderSpawnX() const;
};