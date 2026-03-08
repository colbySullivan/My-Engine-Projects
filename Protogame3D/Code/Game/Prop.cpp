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
	m_orientation.m_pitchDegrees += m_angularVelocity.m_pitchDegrees * deltaSeconds;
	m_orientation.m_rollDegrees += m_angularVelocity.m_rollDegrees * deltaSeconds;
	m_orientation.m_yawDegrees += m_angularVelocity.m_yawDegrees * deltaSeconds;
}

//-----------------------------------------------------------------------------------------------
void Prop::Render() const
{
	g_engine->m_render->SetModelConstants( GetModelToWorldTransform(), m_color );
	g_engine->m_render->m_desiredBlendMode = BlendMode::OPAQUE;
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	g_engine->m_render->BindTexture( m_texture );
	g_engine->m_render->DrawVertexArray( ( int )m_vertexes.size(), m_vertexes.data() );
}

//-----------------------------------------------------------------------------------------------
void Prop::MakeCube( Rgba8 posX /*= Rgba8( 255, 255, 255 )*/, Rgba8 negX /*= Rgba8( 255, 255, 255 )*/, Rgba8 posY /*= Rgba8( 255, 255, 255 )*/, 
	Rgba8 negY /*= Rgba8( 255, 255, 255 )*/, Rgba8 posZ /*= Rgba8( 255, 255, 255 )*/, Rgba8 negZ /*= Rgba8( 255, 255, 255 )*/, Vec3 scale /*= 1 */ )

{
	// +X
	AddVertsForQuad3D( m_vertexes,
		Vec3( 0.5f * scale.x, -0.5f * scale.y, -0.5f * scale.z ),
		Vec3( 0.5f * scale.x, 0.5f * scale.y, -0.5f * scale.z ),
		Vec3( 0.5f * scale.x, 0.5f * scale.y, 0.5f * scale.z ),
		Vec3( 0.5f * scale.x, -0.5f * scale.y, 0.5f * scale.z ),
		posX );			  

	// -X
	AddVertsForQuad3D( m_vertexes,
		Vec3( -0.5f * scale.x, 0.5f * scale.y, -0.5f * scale.z ),
		Vec3( -0.5f * scale.x, -0.5f * scale.y, -0.5f * scale.z ),
		Vec3( -0.5f * scale.x, -0.5f * scale.y, 0.5f * scale.z ),
		Vec3( -0.5f * scale.x, 0.5f * scale.y, 0.5f * scale.z ),
		negX );

	// +Y
	AddVertsForQuad3D( m_vertexes,
		Vec3( 0.5f * scale.x, 0.5f * scale.y, -0.5f * scale.z ),
		Vec3( -0.5f * scale.x, 0.5f * scale.y, -0.5f * scale.z ),
		Vec3( -0.5f * scale.x, 0.5f * scale.y, 0.5f * scale.z ),
		Vec3( 0.5f * scale.x, 0.5f * scale.y, 0.5f * scale.z ),
		posY );

	// -Y
	AddVertsForQuad3D( m_vertexes,
		Vec3( -0.5f * scale.x, -0.5f * scale.y, -0.5f * scale.z ),
		Vec3( 0.5f * scale.x, -0.5f * scale.y, -0.5f * scale.z ),
		Vec3( 0.5f * scale.x, -0.5f * scale.y, 0.5f * scale.z ),
		Vec3( -0.5f * scale.x, -0.5f * scale.y, 0.5f * scale.z ),
		negY );

	// -Z
	AddVertsForQuad3D( m_vertexes,
		Vec3( 0.5f * scale.x, -0.5f * scale.y, -0.5f * scale.z ),
		Vec3( -0.5f * scale.x, -0.5f * scale.y, -0.5f * scale.z ),
		Vec3( -0.5f * scale.x, 0.5f * scale.y, -0.5f * scale.z ),
		Vec3( 0.5f * scale.x, 0.5f * scale.y, -0.5f * scale.z ),
		negZ );

	// +Z
	AddVertsForQuad3D( m_vertexes,
		Vec3( -0.5f * scale.x, -0.5f * scale.y, 0.5f * scale.z ),
		Vec3( 0.5f * scale.x, -0.5f * scale.y, 0.5f * scale.z ),
		Vec3( 0.5f * scale.x, 0.5f * scale.y, 0.5f * scale.z ),
		Vec3( -0.5f * scale.x, 0.5f * scale.y, 0.5f * scale.z ),
		posZ );
}

//------------------------------------------------------------------------------
void Prop::MakeCube( Rgba8 mainColor, Vec3 scale /*= Vec3(1,1,1) */ )
{
	MakeCube( mainColor, mainColor, mainColor, mainColor, mainColor, mainColor, scale );
}

//------------------------------------------------------------------------------
void Prop::MakeCubeAtPos( Rgba8 color, Vec3 size, Vec3 position )
{
	MakeCube( color, size );
	for ( int i = ( int )m_vertexes.size() - 36; i < ( int )m_vertexes.size(); ++i )
	{
		m_vertexes[i].m_position += position;
	}
}

//-----------------------------------------------------------------------------------------------
void Prop::MakeSphere( Vec3 center, float radius, int numSlices, int numStacks )
{
	AddVertsForSphere3D( m_vertexes, center, radius, numSlices, numStacks, m_color );
}

//------------------------------------------------------------------------------
void Prop::MakeCylinder( Vec3 start, Vec3 end, float radius, int numSlices )
{
	AddVertsForCylinder3D( m_vertexes, start, end, radius, m_color, AABB2::ZERO_TO_ONE, numSlices );
}

//------------------------------------------------------------------------------
void Prop::MakeCone( Vec3 start, Vec3 end, float radius, int numSlices )
{
	AddVertsForCone3D( m_vertexes, start, end, radius, m_color, AABB2::ZERO_TO_ONE, numSlices );
}
