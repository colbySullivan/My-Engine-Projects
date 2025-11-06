#include "Game/Bullet.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"

//-----------------------------------------------------------------------------------------------
Bullet::Bullet( Game* owner, Vec2 const& startPos )
	: Entity( owner, startPos )
{
	m_physicsRadius = 0.4f;
	m_cosmeticRadius = 0.5f;
	m_isPushedByWalls = true;
	m_isPushedByEntities = true;
	m_doesPushEntities = true;
	m_isHitByBullets = true;
	m_wanderTimer = 2.0f;
	m_bodyTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/FriendlyShell.png" );
	m_entityType = ENTITY_TYPE_GOOD_BULLET;
	g_engine->m_render->BindTexture( nullptr );
}

//-----------------------------------------------------------------------------------------------
Bullet::~Bullet()
{

}


//-----------------------------------------------------------------------------------------------
void Bullet::Update( float deltaSeconds )
{
	Entity::Update( deltaSeconds );
}

void Bullet::Render() const
{
	if ( m_isDead )
		return;

	std::vector<Vertex> bodyVerts;
	AddVertsForMe( bodyVerts ); // TODO need to make rect
	TransformVertexArrayXY3D( ( int )bodyVerts.size(), bodyVerts.data(), .1f, m_orientationDegrees, m_position );
	g_engine->m_render->BindTexture( m_bodyTexture );
	g_engine->m_render->DrawVertexArray( ( int )bodyVerts.size(), bodyVerts.data() );

	g_engine->m_render->BindTexture( nullptr );

	if ( m_game->g_drawDebug )
	{
		DebugRender();
	}
}

//-----------------------------------------------------------------------------------------------
void Bullet::AddVertsForMe( std::vector<Vertex>& verts ) const
{
	Vec2 mins( -1.f, -0.5f );
	Vec2 maxs( 2.f, 0.5f );
	AABB2 localBox( mins, maxs );
	AddVertsForAABB2D( verts, localBox, Rgba8( 255, 255, 255, 255 ) );
}

//-----------------------------------------------------------------------------------------------
void Bullet::Shoot()
{

}

//-----------------------------------------------------------------------------------------------
void Bullet::DriveForward( float deltaSeconds )
{

}


