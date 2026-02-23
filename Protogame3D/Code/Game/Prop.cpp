#include "Game/Prop.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"

//------------------------------------------------------------------------------
Prop::Prop( Game* owner )
	: Entity( owner )
{
}

//-----------------------------------------------------------------------------------------------
void Prop::Update( [[maybe_unused]] float deltaSeconds )
{
	m_orientation += m_angularVelocity;
}

//-----------------------------------------------------------------------------------------------
void Prop::Render() const
{
	g_engine->m_render->SetModelConstants( GetModelToWorldTransform(), m_color );
	g_engine->m_render->SetBlendMode(BlendMode::OPAQUE);
	g_engine->m_render->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
	g_engine->m_render->BindTexture( m_texture );
	g_engine->m_render->DrawVertexArray( ( int )m_vertexes.size(), m_vertexes.data() );
}

//-----------------------------------------------------------------------------------------------
void Prop::MakeCube( Rgba8 posX, Rgba8 negX, Rgba8 posY, Rgba8 negY, Rgba8 posZ, Rgba8 negZ )
{
	// +X Red
	AddVertsForQuad3D( m_vertexes,
		Vec3( 0.5f, -0.5f, -0.5f ),
		Vec3( 0.5f, 0.5f, -0.5f ),
		Vec3( 0.5f, 0.5f, 0.5f ),
		Vec3( 0.5f, -0.5f, 0.5f ),
		posX );

	// -X Cyan
	AddVertsForQuad3D( m_vertexes,
		Vec3( -0.5f, 0.5f, -0.5f ),
		Vec3( -0.5f, -0.5f, -0.5f ),
		Vec3( -0.5f, -0.5f, 0.5f ),
		Vec3( -0.5f, 0.5f, 0.5f ),
		negX );

	// +Y Green
	AddVertsForQuad3D( m_vertexes,
		Vec3( 0.5f, 0.5f, -0.5f ),
		Vec3( -0.5f, 0.5f, -0.5f ),
		Vec3( -0.5f, 0.5f, 0.5f ),
		Vec3( 0.5f, 0.5f, 0.5f ),
		posY );

	// -Y Magenta
	AddVertsForQuad3D( m_vertexes,
		Vec3( -0.5f, -0.5f, -0.5f ),
		Vec3( 0.5f, -0.5f, -0.5f ),
		Vec3( 0.5f, -0.5f, 0.5f ),
		Vec3( -0.5f, -0.5f, 0.5f ),
		negY );

	// -Z Yellow (bottom)
	AddVertsForQuad3D( m_vertexes,
		Vec3( 0.5f, -0.5f, -0.5f ),
		Vec3( -0.5f, -0.5f, -0.5f ),
		Vec3( -0.5f, 0.5f, -0.5f ),
		Vec3( 0.5f, 0.5f, -0.5f ),
		negZ );

	// +Z Blue (top)
	AddVertsForQuad3D( m_vertexes,
		Vec3( -0.5f, -0.5f, 0.5f ),
		Vec3( 0.5f, -0.5f, 0.5f ),
		Vec3( 0.5f, 0.5f, 0.5f ),
		Vec3( -0.5f, 0.5f, 0.5f ),
		posZ );
}
