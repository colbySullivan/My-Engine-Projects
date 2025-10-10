#include "Bullet.hpp"
#include <Engine/Core/VertexUtils.hpp>
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"


Bullet::Bullet(Game* owner, Vec2 const& startPos)
	: Entity( owner, startPos)
{
	m_physicsRadius = BULLET_PHYSICS_RADIUS;
	m_cosmeticRadius = BULLET_COSMETIC_RADIUS;
	m_debrisSize = 0.05f;
	m_velocity = GetForwardNormal() * BULLET_SPEED;
	InitializeLocalVerts();
}

//-----------------------------------------------------------------------------------------------
Bullet::~Bullet()
{

}

//-----------------------------------------------------------------------------------------------
void Bullet::Update(float deltaSeconds)
{
	m_position += m_velocity * deltaSeconds;
	Entity::Update(deltaSeconds);

	if (IsOffscreen())
	{
		m_isDead = true;
		m_isGarbage = true;
	}
}

//-----------------------------------------------------------------------------------------------
void Bullet::Render() const
{
	if (m_isDead)
	{
		return;
	}
		
	Vertex tempWorldVerts[NUM_BULLET_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_BULLET_VERTS; ++vertIndex)
	{
		tempWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}
	TransformVertexArrayXY3D(NUM_BULLET_VERTS, tempWorldVerts, 2.f, m_orientationDegrees, m_position);
	g_engine->m_render->DrawVertexArray(NUM_BULLET_VERTS, tempWorldVerts);

	if (m_game->g_drawDebug)
		DebugRender();
}

//-----------------------------------------------------------------------------------------------
void Bullet::InitializeLocalVerts()
{
	m_localVerts[0].m_position = Vec3(0.5f, 0.0f, 0.0f);
	m_localVerts[1].m_position = Vec3(0.0f, 0.5f, 0.0f);
	m_localVerts[2].m_position = Vec3(0.0f, -0.5f, 0.0f);
	m_localVerts[0].m_color = Rgba8(255, 255, 255, 255);
	m_localVerts[1].m_color = Rgba8(255, 255, 0, 255);
	m_localVerts[2].m_color = Rgba8(255, 255, 0, 255);

	m_localVerts[3].m_position = Vec3(0.0f, -0.5f, 0.0f);
	m_localVerts[4].m_position = Vec3(0.0f, 0.5f, 0.0f);
	m_localVerts[5].m_position = Vec3(-2.0f, 0.0f, 0.0f);
	m_localVerts[3].m_color = Rgba8(255, 0, 0, 255);
	m_localVerts[4].m_color = Rgba8(255, 0, 0, 255);
	m_localVerts[5].m_color = Rgba8(255, 0, 0, 0);
}

//-----------------------------------------------------------------------------------------------
void Bullet::Die()
{
	SoundPlaybackID temp = g_engine->m_audio->StartSound( 2 );
	m_game->HandleSound( temp );
	m_isGarbage = true;
	m_isDead = true;
	g_theApp->m_game->SpawnDebrisCluster( m_position, m_entityColor, m_velocity, m_debrisAmount, m_debrisSize );
}