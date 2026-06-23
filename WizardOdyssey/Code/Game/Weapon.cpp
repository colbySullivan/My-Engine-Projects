#include "Game/Weapon.hpp"
#include "Game/Entity.hpp"
#include "Game/Map.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"

static constexpr int   MAX_SLOTS = 64;

//------------------------------------------------------------------------------
Weapon::Weapon( const WeaponDefinition* def, int slotIndex )
	: m_def( def )
	, m_slotIndex( slotIndex )
{
	m_texture = g_engine->m_render->CreateOrGetTextureFromFile( m_def->m_spritePath.c_str() );
}

//------------------------------------------------------------------------------
Weapon::~Weapon()
{

}

//------------------------------------------------------------------------------
void Weapon::Update( float deltaSeconds, Vec2 playerPos, float fireOrientation, EntityFaction faction, Map* map, Entity* shooter, int numOfWeapons )
{
	m_timeSinceLastShot -= deltaSeconds;

	if ( m_timeSinceLastShot <= 0.f )
	{
		Vec2 orbitPos = GetOrbitPosition( playerPos, numOfWeapons );
		Entity* bullet = map->SpawnNewEntity( ENTITY_TYPE_GOOD_BULLET, orbitPos, fireOrientation, faction );

		if ( bullet )
		{
			bullet->m_damage = m_def->m_damage;
			bullet->m_shooter = shooter;  // pass player so lifesteal still works
		}

		m_timeSinceLastShot = m_def->m_bulletCooldown;
	}
}

//------------------------------------------------------------------------------
void Weapon::Render( Vec2 playerPos, Vec2 mouseWorldPos, int numOfWeapons ) const
{
	Vec2  orbitPos = GetOrbitPosition( playerPos, numOfWeapons );
	Vec2  toMouse = mouseWorldPos - orbitPos;
	float angle = toMouse.GetOrientationDegrees();

	std::vector<Vertex> verts;
	AABB2 localBox( Vec2( -0.6f, -0.3f ), Vec2( 0.6f, 0.3f ) );
	AddVertsForAABB2D( verts, localBox, Rgba8( 255, 255, 255 ) );

	TransformVertexArrayXY3D( ( int )verts.size(), verts.data(), 1.f, angle, orbitPos );

	g_engine->m_render->BindTexture( m_texture );
	g_engine->m_render->DrawVertexArray( verts );
	g_engine->m_render->BindTexture( nullptr );
}

//------------------------------------------------------------------------------
Vec2 Weapon::GetOrbitPosition( Vec2 playerPos, int numOfWeapons ) const
{
	float angle = ( 360.f / numOfWeapons ) * m_slotIndex;
	return playerPos + Vec2::MakeFromPolarDegrees( angle );
}