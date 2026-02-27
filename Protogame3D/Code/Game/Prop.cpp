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
	g_engine->m_render->SetBlendMode(BlendMode::OPAQUE);
	g_engine->m_render->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
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

void Prop::MakeCube( Rgba8 mainColor, Vec3 scale /*= Vec3(1,1,1) */ )
{
	MakeCube( mainColor, mainColor, mainColor, mainColor, mainColor, mainColor, scale );
}

//-----------------------------------------------------------------------------------------------
void Prop::MakeSphere( Vec3 center, float radius, int numSlices, int numStacks )
{
	float degreesPerStack = 180.f / numStacks;
	float degreesPerSlice = 360.f / numSlices;
	for (int i = 0; i < numStacks ; ++i)
	{
		for ( int j = 0; j < numSlices; j++ ) 
		{
			float leftDegrees = j * degreesPerSlice;
			float rightDegrees = ( j + 1 ) * degreesPerSlice;
			float bottomDegrees = ( i * degreesPerStack ) - 90;
			float topDegrees = ( ( i + 1 ) * degreesPerStack ) - 90;

			Vec3 bl = center + Vec3::MakeFromPolarDegrees( leftDegrees, bottomDegrees ) * radius;
			Vec3 br = center + Vec3::MakeFromPolarDegrees( rightDegrees, bottomDegrees ) * radius;
			Vec3 tr = center + Vec3::MakeFromPolarDegrees( rightDegrees, topDegrees ) * radius;
			Vec3 tl = center + Vec3::MakeFromPolarDegrees( leftDegrees, topDegrees ) * radius;

			float bv = 1.0f - (float)i / (float)numStacks;
			float tv = 1.0f - (float)( i + 1 ) / (float)numStacks;
			float lu = (float)j / (float)numSlices;
			float ru = (float)( j + 1 ) / (float)numSlices;

			m_vertexes.push_back( Vertex( ( bl ), m_color, Vec2( lu, bv ) ) );
			m_vertexes.push_back( Vertex( ( tr ), m_color, Vec2( ru, tv ) ) );
			m_vertexes.push_back( Vertex( ( br ), m_color, Vec2( ru, bv ) ) );

			m_vertexes.push_back( Vertex( ( bl ), m_color, Vec2( lu, bv ) ) );
			m_vertexes.push_back( Vertex( ( tl ), m_color, Vec2( lu, tv ) ) );
			m_vertexes.push_back( Vertex( ( tr ), m_color, Vec2( ru, tv ) ) );
		}
	}
}