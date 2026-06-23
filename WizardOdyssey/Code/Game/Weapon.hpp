#pragma once
#include "Game/WeaponDefinition.hpp"
#include "Game/Entity.hpp"

class Weapon
{
public:
	Weapon( const WeaponDefinition* def, int slotIndex );
	~Weapon();

	void Update( float deltaSeconds, Vec2 playerPos, float fireOrientation, EntityFaction faction, Map* map, Entity* shooter, int numOfWeapons );
	void Render( Vec2 playerPos, Vec2 mouseWorldPos, int numOfWeapons ) const;

private:
	Vec2 GetOrbitPosition( Vec2 playerPos, int numOfWeapons ) const;

	const WeaponDefinition* m_def = nullptr;
	int                     m_slotIndex = 0;
	float                   m_timeSinceLastShot = 0.f;
	Texture*				m_texture = nullptr;
};